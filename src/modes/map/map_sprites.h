///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_sprites.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for map mode sprite code.
*** *****************************************************************************/

#ifndef __MAP_SPRITES_HEADER__
#define __MAP_SPRITES_HEADER__

// Allacrost utilities
#include "utils.h"
#include "defs.h"

// Allacrost engines
#include "video.h"

// Local map mode headers
#include "map_utils.h"
#include "map_objects.h"
#include "map_dialogue.h"
#include "map_zones.h"

namespace hoa_map {

namespace private_map {

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
class VirtualSprite : public MapObject {
public:
	VirtualSprite();

	~VirtualSprite();

	// ---------- Public Members: Orientation and Movement

	/** \brief A bit-mask for the sprite's draw orientation and direction vector.
	*** This member determines both where to move the sprite (8 directions) and
	*** which way the sprite is facing (4 directions). See the Sprite direction
	*** constants for the values that this member may be set to.
	**/
	uint16 direction;

	//! \brief The speed at which the sprite moves around the map.
	float movement_speed;

	/** \brief Set to true when the sprite is currently in motio.
	*** This does not necessarily mean that the sprite actually is moving, but rather
	*** that the sprite is <i>trying</i> to move in a certain direction.
	**/
	bool moving;

	/** \brief Set to true whenever the sprite's position was changed due to movement
	*** This is distinctly different than the moving member. Whereas the moving member
	*** indicates desired movement, this member indicates that positional change due to
	*** movement actually occurred. It is used for drawing functions to determine if they
	*** should draw the sprite in motion or not in motion
	**/
	bool moved_position;

	//! \brief Set to true when the sprite is running rather than walking
	bool is_running;

	// ---------- Public Members: Events

	//! \brief A pointer to the event that is controlling the action of this sprite
	SpriteEvent* control_event;

	// ---------- Public methods

	//! \brief Updates the virtual object's position if it is moving, otherwise does nothing.
	virtual void Update();

	//! \brief Does nothing since virtual sprites have no image to draw
	virtual void Draw()
		{}

	/** \note This method takes into account the current direction when setting the new direction
	*** in the case of diagonal movement. For example, if the sprite is currently facing north
	*** and this function indicates that the sprite should move northwest, it will face north
	*** during the northwest movement.
	**/
	void SetDirection(uint16 dir);

	/** \brief Sets the sprite's direction to a random value
	*** This function is used mostly for the ActionRandomMove class.
	**/
	void SetRandomDirection();

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
	*** member to NULL) before a new event acquires it. The acquisition will be successful regardless
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
	bool IsStateSaved() const
		{ return _state_saved; }

	void SetMovementSpeed(float speed)
		{ movement_speed = speed; }

	void SetMoving(bool motion)
		{ moving = motion; }

	bool GetMoving() const
		{ return moving; }

	uint16 GetDirection() const
		{ return direction; }

	float GetMovementSpeed() const
		{ return movement_speed; }
	//@}

protected:
	/** \brief Determines an appropriate resolution when the sprite collides with an obstruction
	*** \param coll_type The type of collision that has occurred
	*** \param coll_obj A pointer to the MapObject that the sprite has collided with, if any
	**/
	void _ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj);

	/** \name Saved state attributes
	*** These attributes are used to save and restore the state of a VirtualSprite
	**/
	//@{
	//! \brief Indicates if the other saved members are valid because the state has recently been saved
	bool _state_saved;
	uint16 _saved_direction;
	float _saved_movement_speed;
	bool _saved_moving;
	//@}
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
class MapSprite : public VirtualSprite {
public:
	MapSprite();

	~MapSprite();

	// ---------- Public methods

	/** \brief Loads the image containing the standard animations for the sprite
	*** \param filename The name of the image file holding the standard walking animations
	*** \return False if there was a problem loading the sprite.
	**/
	bool LoadStandardAnimations(std::string filename);

	/** \brief Loads the image containing the running animations for the sprite
	*** \param filename The name of the image file
	*** \return False if the animations were not created successfully.
	**/
	bool LoadRunningAnimations(std::string filename);

	/** \brief Loads the image containing the attack animations for the sprite
	*** \param filename The name of the image file
	*** \return False if the animations were not created successfully.
	**/
	bool LoadAttackAnimations(std::string filename);

	void LoadFacePortrait(std::string pn);

	//! \brief Updates the sprite's position and state.
	virtual void Update();

	//! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
	virtual void Draw();

    //! \brief Draws the dialogue icon at the top of the sprite
    virtual void DrawDialog();

	/** \brief Adds a new reference to a dialogue that the sprite uses
	*** \param dialogue_id The ID number of the dialogue
	*** \note It is valid for a dialogue to be referenced more than once by a sprite
	**/
	void AddDialogueReference(uint32 dialogue_id);

    /** \brief Removes all dialogue references from a sprite
    **/
	void ClearDialogueReferences();

	/** \brief Removes a specific dialogue reference from a sprite
	*** \param dialogue_id The ID number of the dialogue that should be removed
	**/
	void RemoveDialogueReference(uint32 dialogue_id);

	/** \brief Begins a new dialogue with this sprite using its next referenced dialogue
	***
	*** If the sprite has no dialogues referenced or has dialogues that are referenced but are unavailable,
	*** a warning will be printed and no dialogue will take place. It is the caller's responsibility to first
	*** check that the sprite has dialogue available.
	**/
	void InitiateDialogue();

	//! \brief Updates all dialogue status members based on the status of all referenced dialogues
	void UpdateDialogueStatus();

	//! \brief Increments the next_dialogue member to index the proceeding dialogue
	void IncrementNextDialogue();

	/** \brief Sets the next dialogue member for the sprite
	*** \param next The index value of the dialogue_references vector to set the next_dialogue member to
	*** \note You can not set the next_dialogue member to a negative number. This could cause run-time errors if it was supported here.
	**/
	void SetNextDialogue(uint16 next);

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
	// TODO: needs to be a ustring
	void SetName(std::string na)
		{ _name = hoa_utils::MakeUnicodeString(na); }

	void SetCurrentAnimation(uint8 anim)
		{ _current_animation = anim; }

	uint8 GetCurrentAnimation() const
		{ return _current_animation; }

	bool HasAvailableDialogue() const
		{ return _has_available_dialogue; }

	bool HasUnseenDialogue() const
		{ return _has_unseen_dialogue; }

	hoa_utils::ustring& GetName()
		{ return _name; }

	hoa_video::StillImage* GetFacePortrait() const
		{ return _face_portrait; }

	//! \brief Returns the next dialogue to reference (negative value returned if no dialogues are referenced)
	int16 GetNextDialogue() const
		{ return _next_dialogue; }

	//! \brief Gets the ID value of the dialogue that will be the next to be referenced by the sprite
	uint32 GetNextDialogueID() const // TODO: check invalid indexing
		{ return _dialogue_references[_next_dialogue]; }

	//! \brief Returns the number of dialogues referenced by the sprite (including duplicates)
	uint16 GetNumberDialogueReferences() const
		{ return _dialogue_references.size(); }

	//! \brief Set to true for a custom animation
	void SetCustomAnimation(bool on_or_off)
		{ _custom_animation_on = on_or_off; }
	//@}


protected:
	//! \brief The name of the sprite, as seen by the player in the game.
	hoa_utils::ustring _name;

	/** \brief A pointer to the face portrait of the sprite, as seen in dialogues and menus.
	*** \note Not all sprites have portraits, in which case this member will be NULL
	**/
	hoa_video::StillImage* _face_portrait;

	//! \brief Set to true if the sprite has running animations loaded
	bool _has_running_animations;

	//! \brief The index to the animations vector containing the current sprite image to display
	uint8 _current_animation;

	/** \brief A vector containing all the sprite's various animations.
	*** The first four entries in this vector are the walking animation frames.
	*** They are ordered from index 0 to 3 as: down, up, left, right. Additional
	*** animations may follow.
	**/
	std::vector<hoa_video::AnimatedImage> _animations;

	//! \brief Contains the id values of all dialogues referenced by the sprite
	std::vector<uint32> _dialogue_references;

	/** \brief An index to the dialogue_references vector, representing the next dialogue the sprite should reference
	*** A negative value indicates that the sprite has no dialogue.
	**/
	int16 _next_dialogue;

	/** \brief True if the sprite references at least one available dialogue
	*** \note A dialogue may become unavailable if it reaches its max view count
	**/
	bool _has_available_dialogue;

	//! \brief True if at least one dialogue referenced by this sprite has not yet been viewed -and- is available to be viewed
	bool _has_unseen_dialogue;

	//! \brief True if a custom animation is in use
	bool _custom_animation_on;

	/** \name Saved state attributes
	*** These attributes are used to save and load the state of a VirtualSprite
	**/
	//@{
	uint8 _saved_current_animation;
	//@}
}; // class MapSprite : public VirtualSprite


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
class EnemySprite : public MapSprite {
private:
	//! \brief The states that the enemy sprite may be in
	enum STATE {
		SPAWNING,
		HOSTILE,
		DEAD
	};

public:

	std::string filename;

	//! \brief The default constructor which typically requires that the user make several additional calls to setup the sprite properties
	EnemySprite();

	//! \brief A constructor for when the enemy sprite is stored in the definition of a single file
	EnemySprite(std::string file);

	//! \brief Loads the enemy's data from a file and returns true if it was successful
	bool Load();

	//! \brief Resets various members of the class so that the enemy is dead, invisible, and does not produce a collision
	void Reset();

	//! \brief Updates the sprite's position and state.
	virtual void Update();

	//! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
	virtual void Draw();

	// TODO: eventually I would like the ability for Lua to pass in a table of ints to the AddEnemyParty function, but because I'm not quite
	// sure how to do that yet, I'm writing several smaller functions so we can just get this demo released.

	// void AddEnemyParty(std::vector<uint32>& party);

	/** \brief Adds a new empty vector to the _enemy_parties member
	*** \note Make sure to populate this vector by adding at least one enemy!
	**/
	void NewEnemyParty()
		{ _enemy_parties.push_back(std::vector<uint32>()); }

	/** \brief Adds an enemy with the specified ID to the last party in _enemy_parties
	*** \param enemy_id The ID of the enemy to add
	*** \note MapMode should have already loaded a GlobalEnemy with this ID and retained it within the MapMode#_enemies member.
	*** If this is not the case, this function will print a warning message.
	**/
	void AddEnemy(uint32 enemy_id);

	//! \brief Returns a reference to a random party of enemies
	const std::vector<uint32>& RetrieveRandomParty();

	//! \name Class Member Access Functions
	//@{
	float GetAggroRange() const
		{ return _aggro_range; }

	uint32 GetTimeToChange() const
		{ return _time_dir_change; }

	uint32 GetTimeToSpawn() const
		{ return _time_to_spawn; }

	std::string GetBattleMusicTheme() const
		{ return _music_theme; }

	std::string GetBattleBackground() const
		{ return _bg_file; }

	std::string GetBattleScript() const
		{ return _script_file; }

	bool IsDead() const
		{ return _state == DEAD; }

	bool IsSpawning() const
		{ return _state == SPAWNING; }

	bool IsHostile() const
		{ return _state == HOSTILE; }

	void SetZone(EnemyZone* zone)
		{ _zone = zone; }

	void SetAggroRange(float range)
		{ _aggro_range = range; }

	void SetTimeToChange(uint32 time)
		{ _time_dir_change = time; }

	void SetTimeToSpawn(uint32 time)
		{ _time_to_spawn = time; }

	void SetBattleMusicTheme(const std::string& music_theme)
		{ _music_theme = music_theme; }

	void SetBattleBackground(const std::string& bg_file)
		{ _bg_file = bg_file; }

	void SetBattleScript(const std::string& script_file)
		{ _script_file = script_file; }

	void ChangeStateDead()
		{ Reset(); if (_zone) _zone->EnemyDead(); }

	void ChangeStateSpawning()
		{ updatable = true; _state = SPAWNING; no_collision = false; }

	void ChangeStateHostile()
		{ updatable = true; _state = HOSTILE; no_collision = false; _color.SetAlpha(1.0); }
	//@}

private:
	//! \brief The zone that the enemy sprite belongs to
	private_map::EnemyZone* _zone;

	//! \brief Used to gradually fade in the sprite as it is spawning by adjusting the alpha channel
	hoa_video::Color _color;

	//! \brief A timer used for spawning
	uint32 _time_elapsed;

	//! \brief The state that the enemy sprite is in
	STATE _state;

	//! \brief A value which determines how close the player needs to be for the enemy to aggressively seek to confront it
	float _aggro_range;

	//! \brief ???
	uint32 _time_dir_change;

	//! \brief ???
	uint32 _time_to_spawn;

	//! \brief Indicates if the enemy is outside of its zone. If it is, it won't change direction until it gets back in.
	bool _out_of_zone;

	//! \brief The default battle music theme for the monster
	std::string _music_theme;

	//! \brief The default background for the battle
	std::string _bg_file;

	//! \brief The filename of the script to pass to the battle
	std::string _script_file;

	/** \brief Contains the possible groups of enemies that may appear in a battle should the player encounter this enemy sprite
	*** The numbers contained within this member are ID numbers for the enemy. If the
	**/
	std::vector<std::vector<uint32> > _enemy_parties;
}; // class EnemySprite : public MapSprite

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_SPRITES_HEADER__
