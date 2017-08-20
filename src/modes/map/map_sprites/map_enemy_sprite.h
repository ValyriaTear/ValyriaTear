///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_ENEMY_SPRITE_HEADER__
#define __MAP_ENEMY_SPRITE_HEADER__

#include "modes/map/map_sprites/map_sprite.h"

#include "modes/battle/battle_enemy_info.h"

namespace vt_map
{

namespace private_map
{

class SpriteEvent;
class EnemyZone;

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
    virtual bool Update();

    //! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
    virtual bool Draw();

    /** \brief Adds a new empty vector to the _enemy_parties member
    *** \note Make sure to populate this vector by adding at least one enemy!
    **/
    void NewEnemyParty() {
        _enemy_parties.push_back(std::vector<vt_battle::BattleEnemyInfo>());
    }

    /** \brief Adds an enemy with the specified ID to the last party in _enemy_parties
    *** \param enemy_id The ID of the enemy to add
    *** \param position_x, position_y The enemy sprite position on the battle ground in pixels
    *** \note MapMode should have already loaded a GlobalEnemy with this ID and retained it within the MapMode#_enemies member.
    *** If this is not the case, this function will print a warning message.
    **/
    bool AddEnemy(uint32_t enemy_id, float position_x, float position_y);
    //! \brief A simpler function used to auto set default enemy position on the battle ground
    bool AddEnemy(uint32_t enemy_id) {
        return AddEnemy(enemy_id, 0.0f, 0.0f);
    }

    //! \brief Returns a reference to a random party of enemies
    const std::vector<vt_battle::BattleEnemyInfo>& RetrieveRandomParty() const;

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

    void ChangeStateDead();

    void ChangeStateSpawning() {
        _updatable = true;
        _state = SPAWNING;
        _collision_mask = NO_COLLISION;
    }

    void ChangeStateHostile();

    //! Makes an enemy follow way point when not running after a hero
    //! \note You'll have to add at least two valid way point to make those
    //! taken into account by the enemy sprite.
    bool AddWayPoint(float destination_x, float destination_y);
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
    std::vector<std::vector<vt_battle::BattleEnemyInfo> > _enemy_parties;

    //! \brief The enemy's encounter event.
    //! If this event is not empty, it is triggered instead of a battle.
    std::string _encounter_event;

    //! \brief Tells whether pathfinding is used to compute the enemy movement.
    bool _use_path;

    //! \brief Used to store the previous coordinates of the sprite during path movement,
    //! so as to set the proper direction of the sprite as it moves
    vt_common::Position2D _last_node_position;

    //! \brief Used to store the current node collision position (with offset)
    vt_common::Position2D _current_node;

    //! \brief An index to the path vector containing the node that the sprite currently occupies
    uint32_t _current_node_id;

    //! \brief The current destination of the sprite.
    vt_common::Position2D _destination;

    //! \brief Holds the path needed to traverse from source to destination
    Path _path;

    //! \brief Way points used by the enemy when not hostile
    std::vector<vt_common::Position2D> _way_points;
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
    bool _UpdatePath();

    //! \brief Set a path for the sprite being the next way point given.
    //! \return whether it failed.
    bool _SetPathToNextWayPoint();

    //! \brief Handles behavior when the enemy is in hostile state (seeking for characters)
    void _HandleHostileUpdate();

};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_ENEMY_SPRITE_HEADER__
