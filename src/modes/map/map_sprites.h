///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2015 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_sprites.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode sprite code.
*** *****************************************************************************/

#ifndef __MAP_SPRITES_HEADER__
#define __MAP_SPRITES_HEADER__

#include "modes/map/map_dialogue.h"
#include "modes/map/map_zones.h"

namespace vt_map
{

namespace private_map
{

class SpriteEvent;

//! Standard time values for spawning enemies on a map. All values are in number of milliseconds.
//@{
//! \brief The time to spawn an enemy when the player first enters a map
const uint32_t STANDARD_ENEMY_FIRST_SPAWN_TIME = 1000;
//! \brief The standard amount of time it takes an enemy to change state from "spawning" to "hostile"
const uint32_t STANDARD_ENEMY_SPAWN_TIME = 5000;
//! \brief The duration that an enemy stays in the dead state after it has been defeated
const uint32_t STANDARD_ENEMY_DEAD_TIME = 5000;
//@}

/** ****************************************************************************
*** \brief A special type of sprite with no physical image
***
*** The VirtualSprite is a special type of MapObject because it has no physical
*** form (no image). Virtual sprites may be manipulated to move around on the screen
*** just like any other sprite. VirtualSprites do take collision detection into account
*** by default, unless the no_collision member is set to true. Here are some examples of
*** where virtual sprites may be of use:
***
*** - As a mobile focusing point for the map camera
*** - As an impassible map location for ground objects in a specific context only
*** - To set impassible locations for objects in the sky layer
***
*** \note The VirtualSprite class serves as a base class for all other types of
*** sprites.
*** ***************************************************************************/
class VirtualSprite : public MapObject
{
public:
    explicit VirtualSprite(MapObjectDrawLayer layer);
    virtual ~VirtualSprite() override;

    //! \brief Updates the virtual object's position if it is moving, otherwise does nothing.
    virtual void Update() override;

    //! \brief Does nothing since virtual sprites have no image to draw
    virtual void Draw() override
    {
    }

    /** \note This method takes into account the current direction when setting the new direction
    *** in the case of diagonal movement. For example, if the sprite is currently facing north
    *** and this function indicates that the sprite should move northwest, it will face north
    *** during the northwest movement.
    **/
    void SetDirection(uint16_t dir);

    /** \brief Sets the sprite's direction to a random value
    *** This function is used mostly for the ActionRandomMove class.
    **/
    void SetRandomDirection();

    /**
    *** Make the sprite used the direction making it "look at" the given position or sprite.
    **/
    void LookAt(const MapPosition &pos);
    void LookAt(float x, float y) {
        LookAt(MapPosition(x, y));
    }
    void LookAt(MapObject *object) {
        if(object) LookAt(object->GetPosition());
    }

    /** \brief Calculates the distance the sprite should move given its velocity (speed and direction)
    *** \return A floating point value representing the distance moved
    *** \note This method does not check if the "moving" member is true but does factor in the "is_running"
    *** member in its calculation.
    **/
    float CalculateDistanceMoved();

    /** \brief Declares that an event is taking control over the sprite
    *** \param event The sprite event that is assuming control
    *** This function is not safe to call when there is an event already controlling the sprite.
    *** The previously controlling event should first release control (which will set the control_event
    *** member to nullptr) before a new event acquires it. The acquisition will be successful regardless
    *** of whether there is currently a controlling event or not, but a warning will be printed in the
    *** improper case.
    **/
    void AcquireControl(SpriteEvent* event);

    /** \brief Declares that an event is releasing control over the sprite
    *** \param event The sprite event that is releasing control
    *** The reason why the SpriteEvent has to pass a pointer to itself in this call is to make sure
    *** that this event is still controlling the sprite. If the control has switched to another event
    *** (because another event acquired it before this event released it), a warning will be printed
    *** and no change will be made (the control event will not change).
    **/
    void ReleaseControl(SpriteEvent* event);

    /** \brief Gets the SpriteEvent* currently controlling the sprite
    *** or nullptr is none.
    **/
    SpriteEvent* GetControlEvent() const {
        return _control_event;
    }

    /** \brief Saves the state of the sprite
    *** Attributes saved: direction, speed, moving state
    **/
    virtual void SaveState();

    /** \brief Restores the saved state of the sprite
    *** Attributes restored: direction, speed, moving state
    **/
    virtual void RestoreState();

    /** \name Lua Access Functions
    *** These functions are specifically written to enable Lua to access the members of this class.
    **/
    //@{
    bool IsStateSaved() const {
        return _state_saved;
    }

    void SetMovementSpeed(float speed) {
        _movement_speed = speed;
    }

    void SetMoving(bool motion) {
        _moving = motion;
    }

    bool GetMoving() const {
        return _moving;
    }

    uint16_t GetDirection() const {
        return _direction;
    }

    float GetMovementSpeed() const {
        return _movement_speed;
    }

    void SetRunning(bool running) {
        _is_running = running;
    }

    bool IsRunning() const {
        return _is_running;
    }

    bool HasMoved() const {
        return _moved_position;
    }
    //@}

protected:
    /** \brief A bit-mask for the sprite's draw orientation and direction vector.
    *** This member determines both where to move the sprite (8 directions) and
    *** which way the sprite is facing (4 directions). See the Sprite direction
    *** constants for the values that this member may be set to.
    **/
    uint16_t _direction;

    //! \brief The speed at which the sprite moves around the map.
    float _movement_speed;

    /** \brief Set to true when the sprite is currently in motion.
    *** This does not necessarily mean that the sprite actually is moving, but rather
    *** that the sprite is <i>trying</i> to move in a certain direction.
    **/
    bool _moving;

    /** \brief Set to true whenever the sprite's position was changed due to movement
    *** This is distinctly different than the moving member. Whereas the moving member
    *** indicates desired movement, this member indicates that positional change due to
    *** movement actually occurred. It is used for drawing functions to determine if they
    *** should draw the sprite in motion or not in motion
    **/
    bool _moved_position;

    //! \brief Set to true when the sprite is running rather than walking
    bool _is_running;

    //! \brief A pointer to the event that is controlling the action of this sprite
    SpriteEvent* _control_event;

    /** \name Saved state attributes
    *** These attributes are used to save and restore the state of a VirtualSprite
    **/
    //@{
    //! \brief Indicates if the other saved members are valid because the state has recently been saved
    bool _state_saved;
    uint16_t _saved_direction;
    float _saved_movement_speed;
    bool _saved_moving;
    //@}

    /** \brief Set the next sprite position, according to the current direction set.
    *** This function aims at finding the next correct position for the given sprite,
    *** and avoid the most possible to make it stop, except when walking against a wall.
    **/
    void _SetNextPosition();

    /** \brief Handles position corrections when the sprite is on the edge of
    *** physical obstacles. (NPC sprites, treasure, ... aren't considered here for playability purpose)
    **/
    bool _HandleWallEdges(float& next_pos_x, float& next_pos_y, float distance_moved,
                          MapObject* collision_object);
}; // class VirtualSprite : public MapObject


/** ****************************************************************************
*** \brief A mobile map object with which the player can interact with.
***
*** Map sprites are animate, mobile, living map objects. Although there is
*** but this single class to represent all the map sprites in the game, they can
*** divided into types such as NPCs, friendly creatures, and enemies. The fact
*** that there is only one class for representing several sprite types is the
*** reason why many of the class members are pointers. For example, we don't
*** need dialogue for a dog sprite.
*** ***************************************************************************/
class MapSprite : public VirtualSprite
{
public:
    explicit MapSprite(MapObjectDrawLayer layer);
    virtual ~MapSprite() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static MapSprite* Create(MapObjectDrawLayer layer);

    // ---------- Public methods

    /** \brief Loads the standing animations of the sprite for the four directions.
    *** \param filename The name of the script animation file holding the standing animations
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadStandingAnimations(const std::string &filename);

    /** \brief Loads the walking animations of the sprite for the four directions.
    *** \param filename The name of the script animation file holding the walking animations
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadWalkingAnimations(const std::string &filename);

    /** \brief Loads the running animations of the sprite for the four directions.
    *** \param filename The name of the image file holding the walking animation
    *** \return False if there was a problem loading the sprites.
    **/
    bool LoadRunningAnimations(const std::string &filename);

    /** \brief Loads the script containing the one-sided custom animation of the sprite.
    *** \param animation_name The animation name of the custom animation.
    *** \param filename The name of the image file holding the given custom animation (one direction only)
    *** \return False if there was a problem loading the animation.
    **/
    bool LoadCustomAnimation(const std::string &animation_name, const std::string& filename);

    //! \brief Clear out all the sprite animation. Useful in case of reloading.
    void ClearAnimations();

    void LoadFacePortrait(const std::string& filename);

    //! \brief Updates the sprite's position and state.
    virtual void Update();

    //! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
    virtual void Draw();

    //! \brief Draws the dialogue icon at the top of the sprite
    virtual void DrawDialogIcon();

    /** \brief Adds a new reference to a dialogue that the sprite uses
    *** \param dialogue The SpriteDialogue used.
    *** \note It is valid for a dialogue to be referenced more than once by a sprite
    **/
    void AddDialogueReference(SpriteDialogue* dialogue);

    /** \brief Removes all dialogue references from a sprite
    **/
    void ClearDialogueReferences();

    /** \brief Removes a specific dialogue reference from a sprite
    *** \param dialogue The SpriteDialogue used.
    **/
    void RemoveDialogueReference(SpriteDialogue* dialogue);

    /** \brief Begins a new dialogue with this sprite using its next referenced dialogue
    ***
    *** If the sprite has no dialogues referenced or has dialogues that are referenced but are unavailable,
    *** a warning will be printed and no dialogue will take place. It is the caller's responsibility to first
    *** check that the sprite has dialogue available.
    **/
    void InitiateDialogue();

    //! \brief Updates all dialogue status members based on the status of all referenced dialogues
    void UpdateDialogueStatus();

    /** \brief Increments the next_dialogue member to index the proceeding dialogue
    *** \note Change from Allacrost: Stay at the last dialogue when at this point.
    *** (Don't go back to the first one as done previously).
    **/
    void IncrementNextDialogue();

    /** \brief Sets the next dialogue member for the sprite
    *** \param next The index value of the dialogue_references vector to set the next_dialogue member to
    *** \note You can not set the next_dialogue member to a negative number. This could cause run-time errors if it was supported here.
    **/
    void SetNextDialogue(uint16_t next);

    /** \brief This method will save the state of a sprite.
    *** Attributes saved: direction, speed, moving state, name
    *** current animation.
    **/
    virtual void SaveState();

    /** \brief This method will load the saved state of a sprite.
    *** Attributes loaded: direction, speed, moving state, name
    *** current animation.
    *** \return false if there was no saved state, true otherwise.
    **/
    virtual void RestoreState();

    /** \name Lua Access Functions
    *** These functions are specifically written to enable Lua to access the members of this class.
    **/
    //@{
    void SetName(const vt_utils::ustring &name) {
        _name = name;
    }

    void SetName(const std::string &name) {
        _name = vt_utils::MakeUnicodeString(name);
    }

    void SetCurrentAnimationDirection(uint8_t anim_direction) {
        _current_anim_direction = anim_direction;
    }

    uint8_t GetCurrentAnimationDirection() const {
        return _current_anim_direction;
    }

    bool HasAvailableDialogue() const {
        return _has_available_dialogue;
    }

    bool HasUnseenDialogue() const {
        return _has_unseen_dialogue;
    }

    vt_utils::ustring &GetName() {
        return _name;
    }

    vt_video::StillImage *GetFacePortrait() const {
        return _face_portrait;
    }

    //! \brief Returns the next dialogue to reference (negative value returned if no dialogues are referenced)
    int16_t GetNextDialogue() const {
        return _next_dialogue;
    }

    //! \brief Gets the ID value of the dialogue that will be the next to be referenced by the sprite
    const std::string& GetNextDialogueID() const;

    //! \brief Returns the number of dialogues referenced by the sprite (including duplicates)
    uint16_t GetNumberDialogueReferences() const {
        return _dialogue_references.size();
    }

    /** \brief Tells the sprite to use a custom animation
    *** \param The animation name used as a key to find the custom animation declared in map_sprites.lua
    *** You can set the animation key to empty to disable the custom animation.
    *** \param The time to display the given animation, -1 for the default time and 0 for an infinite amount of time.
    **/
    void SetCustomAnimation(const std::string &animaton_name, int32_t time);

    bool IsAnimationCustom() const {
        return _custom_animation_on;
    }

    /** \brief Disable a posible running custom animation.
    *** Useful after setting an inifinite running animation, for instance.
    **/
    void DisableCustomAnimation() {
        _custom_animation_on = false;
        _infinite_custom_animation = false;
    }

    void SetSpriteName(const std::string &map_sprite_name) {
        _sprite_name = map_sprite_name;
    }

    const std::string &GetSpriteName() const {
        return _sprite_name;
    }

    /** \brief Used to reload (or change) the graphic animations
    *** of an existing sprite.
    *** \param sprite_name The sprite name entry found in the map_sprites.lua
    *** sprite table.
    **/
    void ReloadSprite(const std::string& sprite_name);

    //! Will change the sprite type to SCENERY,
    //! making other sprites unable to collide with it. It is usually used to setup
    //! harmless and little animals seen on maps.
    void SetSpriteAsScenery(bool is_scenery) {
        MapObject::_object_type = is_scenery ? SCENERY_TYPE : SPRITE_TYPE;
    }

    //! \brief Sets/unsets the sprite animations as grayscale.
    virtual void SetGrayscale(bool grayscale = true);
    //@}

protected:
    //! \brief The name of the sprite, as seen by the player in the game.
    vt_utils::ustring _name;

    /** \brief A pointer to the face portrait of the sprite, as seen in dialogues and menus.
    *** \note Not all sprites have portraits, in which case this member will be nullptr
    **/
    vt_video::StillImage *_face_portrait;

    /** Keeps the map sprite reference name permitting, used to know whether a map sprite needs reloading
    *** when the map sprite name has actually changed.
    **/
    std::string _sprite_name;

    //! \brief Set to true if the sprite has running animations loaded
    bool _has_running_animations;

    //! \brief The current sprite direction. (for animation)
    uint8_t _current_anim_direction;

    //! \brief A map containing all four directions of the sprite's various animations.
    std::vector<vt_video::AnimatedImage> _standing_animations;
    std::vector<vt_video::AnimatedImage> _walking_animations;
    std::vector<vt_video::AnimatedImage> _running_animations;

    //! \brief A pointer to the current standard animation vector
    std::vector<vt_video::AnimatedImage>* _animation;

    //! \brief A map containing all the custom animations, indexed by their name.
    std::map<std::string, vt_video::AnimatedImage> _custom_animations;

    //! \brief The currently used custom animation.
    vt_video::AnimatedImage *_current_custom_animation;

    //! \brief Contains the id values of all dialogues referenced by the sprite
    std::vector<std::string> _dialogue_references;

    /** \brief An index to the dialogue_references vector, representing the next dialogue the sprite should reference
    *** A negative value indicates that the sprite has no dialogue.
    **/
    int16_t _next_dialogue;

    //! \brief True if the sprite references at least one dialogue
    bool _has_available_dialogue;

    //! \brief True if at least one dialogue referenced by this sprite has not yet been viewed -and- is available to be viewed
    bool _has_unseen_dialogue;

    //! \brief true if the sprite is talking with the camera.
    bool _dialogue_started;

    //! \brief True if a custom animation is in use
    bool _custom_animation_on;

    //! \brief Tells how much time left the custom animation will have to be drawn
    int32_t _custom_animation_time;

    //! Tells whether the animation has got an infinite duration
    bool _infinite_custom_animation;

    /** \name Saved state attributes
    *** These attributes are used to save and load the state of a VirtualSprite
    **/
    //@{
    uint8_t _saved_current_anim_direction;
    //@}

    //! \brief Draws debug information, used for pathfinding mostly.
    void _DrawDebugInfo();
}; // class MapSprite : public VirtualSprite

//! \brief Data used to load an place enemies on battle grounds
struct BattleEnemyInfo {
    BattleEnemyInfo():
        enemy_id(0),
        position_x(0.0f),
        position_y(0.0f)
    {}

    BattleEnemyInfo(uint32_t id, float x, float y):
        enemy_id(id),
        position_x(x),
        position_y(y)
    {}

    //! \brief  The enemy id see in enemies.lua
    uint32_t enemy_id;

    //! \brief The enemy position in the battle ground, in pixels.
    float position_x;
    float position_y;
};

/** ****************************************************************************
*** \brief A mobile map object that induces a battle to occur if the player touches it
***
*** There are really two types of enemy sprites. The first type behave just like
*** map sprites and can have scripted movement sequences. The second type belong
*** to EnemyZones, where they fade into existence and pursue after the player's
*** sprite should the player enter the zone.
***
*** An enemy sprite in a zone can be in one of 3 states: SPAWNING, HOSTILE or DEAD.
*** In the spawning state, the enemy becomes gradually visible, is immobile, and
*** cannot be touched or attacked. In the hostile state, the enemies roams the map
*** and will cause a battle if touched by the player. In the dead state, the enemy
*** is invisible and waits for the EnemyZone to reset it in another position, so
*** that it may spawn once more.
*** ***************************************************************************/
class EnemySprite : public MapSprite
{
private:
    //! \brief The states that the enemy sprite may be in
    enum STATE {
        SPAWNING,
        HOSTILE,
        DEAD
    };

public:
    //! \brief The default constructor
    EnemySprite();
    virtual ~EnemySprite() override;

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static EnemySprite* Create();

    //! \brief Resets various members of the class so that the enemy is dead, invisible, and does not produce a collision
    void Reset();

    //! \brief Updates the sprite's position and state.
    virtual void Update();

    //! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
    virtual void Draw();

    /** \brief Adds a new empty vector to the _enemy_parties member
    *** \note Make sure to populate this vector by adding at least one enemy!
    **/
    void NewEnemyParty() {
        _enemy_parties.push_back(std::vector<BattleEnemyInfo>());
    }

    /** \brief Adds an enemy with the specified ID to the last party in _enemy_parties
    *** \param enemy_id The ID of the enemy to add
    *** \param position_x, position_y The enemy sprite position on the battle ground in pixels
    *** \note MapMode should have already loaded a GlobalEnemy with this ID and retained it within the MapMode#_enemies member.
    *** If this is not the case, this function will print a warning message.
    **/
    void AddEnemy(uint32_t enemy_id, float position_x, float position_y);
    //! \brief A simpler function used to auto set default enemy position on the battle ground
    void AddEnemy(uint32_t enemy_id) {
        AddEnemy(enemy_id, 0.0f, 0.0f);
    }

    //! \brief Returns a reference to a random party of enemies
    const std::vector<BattleEnemyInfo>& RetrieveRandomParty() const;

    //! \brief Returns the enemy's encounter event id.
    //! If this event is not empty, it is triggered instead of a battle,
    //! when encountering an enemy sprite in the map mode.
    const std::string& GetEncounterEvent() const {
        return _encounter_event;
    }

    //! \brief Sets the enemy's encounter event id.
    void SetEncounterEvent(const std::string& event) {
        _encounter_event = event;
    }

    //! \name Class Member Access Functions
    //@{
    float GetAggroRange() const {
        return _aggro_range;
    }

    uint32_t GetTimeBeforeNewDestination() const {
        return _time_before_new_destination;
    }

    uint32_t GetTimeToSpawn() const {
        return _time_to_spawn;
    }

    const std::string &GetBattleMusicTheme() const {
        return _music_theme;
    }

    const std::string &GetBattleBackground() const {
        return _bg_file;
    }

    const std::vector<std::string>& GetBattleScripts() const {
        return _script_files;
    }

    bool IsDead() const {
        return _state == DEAD;
    }

    bool IsSpawning() const {
        return _state == SPAWNING;
    }

    bool IsHostile() const {
        return _state == HOSTILE;
    }

    bool IsBoss() const {
        return _is_boss;
    }

    void SetZone(EnemyZone *zone) {
        _zone = zone;
    }

    EnemyZone* GetEnemyZone() {
        return _zone;
    }

    void SetAggroRange(float range) {
        _aggro_range = range;
    }

    void SetTimeBeforeNewDestination(uint32_t time) {
        _time_before_new_destination = time;
    }

    void SetTimeToRespawn(uint32_t time) {
        _time_to_respawn = time;
    }

    void SetBoss(bool is_boss) {
        _is_boss = is_boss;
    }

    void SetBattleMusicTheme(const std::string &music_theme) {
        _music_theme = music_theme;
    }

    void SetBattleBackground(const std::string &bg_file) {
        _bg_file = bg_file;
    }

    void AddBattleScript(const std::string &script_file) {
        _script_files.push_back(script_file);
    }

    void ChangeStateDead() {
        Reset();
        if(_zone) _zone->EnemyDead();
    }

    void ChangeStateSpawning() {
        _updatable = true;
        _state = SPAWNING;
        _collision_mask = NO_COLLISION;
    }

    void ChangeStateHostile();

    //! Makes an enemy follow way point when not running after a hero
    //! \note You'll have to add at least two valid way point to make those
    //! taken into account by the enemy sprite.
    void AddWayPoint(float destination_x, float destination_y);
    //@}

private:
    //! \brief The zone that the enemy sprite belongs to
    private_map::EnemyZone *_zone;

    //! \brief Used to gradually fade in the sprite as it is spawning by adjusting the alpha channel
    vt_video::Color _color;

    //! \brief A timer used for spawning
    uint32_t _time_elapsed;

    //! \brief The state that the enemy sprite is in
    STATE _state;

    //! \brief A value which determines how close the player needs to be for the enemy to aggressively seek to confront it
    float _aggro_range;

    //! \brief Tells the time the sprite is waiting before going to a new destination.
    uint32_t _time_before_new_destination;

    //! \brief Tells the actual time in milliseconds the sprite will use to respawn. This will set up the fade in speed.
    uint32_t _time_to_spawn;

    //! \brief the default time used to respawn (Set to STANDARD_ENEMY_SPAWN_TIME by default)
    uint32_t _time_to_respawn;

    //! \brief The default battle music theme for the monster
    std::string _music_theme;

    //! \brief The default background for the battle
    std::string _bg_file;

    //! \brief The filenames of the script to pass to the battle
    std::vector<std::string> _script_files;

    //! \brief Tells whether the sprite is a boss.
    bool _is_boss;

    /** \brief Contains the possible groups of enemies that may appear in a battle should the player encounter this enemy sprite
    *** The numbers contained within this member are ID numbers for the enemy.
    **/
    std::vector<std::vector<BattleEnemyInfo> > _enemy_parties;

    //! \brief The enemy's encounter event.
    //! If this event is not empty, it is triggered instead of a battle.
    std::string _encounter_event;

    //! \brief Tells whether pathfinding is used to compute the enemy movement.
    bool _use_path;

    //! \brief Used to store the previous coordinates of the sprite during path movement,
    //! so as to set the proper direction of the sprite as it moves
    float _last_node_x_position, _last_node_y_position;

    //! \brief Used to store the current node collision position (with offset)
    float _current_node_x, _current_node_y;

    //! \brief An index to the path vector containing the node that the sprite currently occupies
    uint32_t _current_node_id;

    //! \brief The current destination of the sprite.
    float _destination_x, _destination_y;

    //! \brief Holds the path needed to traverse from source to destination
    Path _path;

    //! \brief Way points used by the enemy when not hostile
    std::vector<MapPosition> _way_points;
    uint32_t _current_way_point_id;

    //! \brief Set the new path destination of the sprite.
    //! \param destination_x The pixel x destination to find a path to.
    //! \param destination_y The pixel y destination to find a path to.
    //! \param max_cost More or less the path max length in nodes or 0 if no limitations.
    //! Use this to avoid heavy computations.
    //! \return whether it failed.
    bool _SetDestination(float destination_x, float destination_y, uint32_t max_cost = 20);

    //! \brief Set the actual sprite direction according to the current path node.
    void _SetSpritePathDirection();

    //! \brief Update the sprite direction according to the current path.
    void _UpdatePath();

    //! \brief Set a path for the sprite being the next way point given.
    //! \return whether it failed.
    bool _SetPathToNextWayPoint();

    //! \brief Handles behavior when the enemy is in hostile state (seeking for characters)
    void _HandleHostileUpdate();

}; // class EnemySprite : public MapSprite

} // namespace private_map

} // namespace vt_map

#endif // __MAP_SPRITES_HEADER__
