///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    sprites.h
*** \author  Dale Ma, dalema22@gmail.com
*** \brief   Header file for editor sprite code.
*** *****************************************************************************/

#ifndef __SPRITES_HEADER__
#define __SPRITES_HEADER__

#include "utils.h"
#include "defs.h"
#include "video.h"

#include <QRectF>
#include <QPointF>

namespace hoa_editor {

// *********************** SPRITE CONSTANTS **************************

/** \name Map Sprite Speeds
*** \brief Common speeds for sprite movement.
*** These values are the time (in milliseconds) that it takes a sprite to walk
*** the distance of one map grid (16 pixels).
**/
//@{
const float VERY_SLOW_SPEED = 225.0f;
const float SLOW_SPEED      = 190.0f;
const float NORMAL_SPEED    = 150.0f;
const float FAST_SPEED      = 110.0f;
const float VERY_FAST_SPEED = 75.0f;
//@}

/** \name Position Transferring Factors
*** \breif Position transfering factor from game to editor
**/
//@{
const float X_POS_FACTOR = 2.0f;
const float Y_POS_FACTOR = 2.0f;
//@}

/** \name Map Object Type Constants
*** These constants are used to identify the type of map object or sprite.
**/
//@{
const uint8 PHYSICAL_TYPE = 0;
const uint8 VIRTUAL_TYPE = 1;
const uint8 SPRITE_TYPE = 2;
const uint8 ENEMY_TYPE = 3;
const uint8 TREASURE_TYPE = 4;
//@}

/** \name Sprite Direction Constants
*** \brief Constants used for setting and determining sprite directions
*** Sprites are allowed to travel in eight different directions, however the sprite itself
*** can only be facing one of four ways: north, south, east, or west. Because of this, it
*** is possible to travel, for instance, northwest facing north <i>or</i> northwest facing west.
*** The "NW_NORTH" constant means that the sprite is traveling to the northwest and is
*** facing towards the north.
***
*** \note The set of "FACING_DIRECTION" and "MOVING_DIRECTION" constants are only meant to be
*** used as shorthands. You shouldn't assign the MapSprite#direction member to any of these values.
**/
//@{
const uint16 NORTH     = 0x0001;
const uint16 SOUTH     = 0x0002;
const uint16 WEST      = 0x0004;
const uint16 EAST      = 0x0008;
const uint16 NW_NORTH  = 0x0010;
const uint16 NW_WEST   = 0x0020;
const uint16 NE_NORTH  = 0x0040;
const uint16 NE_EAST   = 0x0080;
const uint16 SW_SOUTH  = 0x0100;
const uint16 SW_WEST   = 0x0200;
const uint16 SE_SOUTH  = 0x0400;
const uint16 SE_EAST   = 0x0800;

const uint16 NORTHWEST = NW_NORTH | NW_WEST;
const uint16 NORTHEAST = NE_NORTH | NE_EAST;
const uint16 SOUTHWEST = SW_SOUTH | SW_WEST;
const uint16 SOUTHEAST = SE_SOUTH | SE_EAST;

const uint16 FACING_NORTH = NORTH | NW_NORTH | NE_NORTH;
const uint16 FACING_SOUTH = SOUTH | SW_SOUTH | SE_SOUTH;
const uint16 FACING_WEST = WEST | NW_WEST | SW_WEST;
const uint16 FACING_EAST = EAST | NE_EAST | SE_EAST;

const uint16 LATERAL_MOVEMENT = NORTH | SOUTH | EAST | WEST;
const uint16 DIAGONAL_MOVEMENT = NORTHWEST | NORTHEAST | SOUTHWEST | SOUTHEAST;
//@}

/** \name Map Sprite Animation Constants
*** These constants are used to index the MapSprite#animations vector to display the correct
*** animation. The first 8 entries in this vector always represent the same sets of animations
*** for each map sprite.
**/
//@{
const uint32 ANIM_STANDING_SOUTH = 0;
const uint32 ANIM_STANDING_NORTH = 1;
const uint32 ANIM_STANDING_WEST  = 2;
const uint32 ANIM_STANDING_EAST  = 3;
const uint32 ANIM_WALKING_SOUTH  = 4;
const uint32 ANIM_WALKING_NORTH  = 5;
const uint32 ANIM_WALKING_WEST   = 6;
const uint32 ANIM_WALKING_EAST   = 7;
const uint32 ANIM_RUNNING_SOUTH  = 8;
const uint32 ANIM_RUNNING_NORTH  = 9;
const uint32 ANIM_RUNNING_WEST   = 10;
const uint32 ANIM_RUNNING_EAST   = 11;
//@}

/** ****************************************************************************
*** \brief An invisible and possible mobile sprite on a map
***
*** The VirtualSprite is a special type of MapObject because it has no physical
*** form (no image). Virtual sprites may be manipulated to move around on the screen,
*** or they may remain stationary. VirtualSprites do take collision detection into account
*** by default, unless the no_collision member is set to true. Here are some examples of
*** where virtual sprites may be of use:
***
*** - As a mobile focusing point for the map camera
*** - As an impassible map location for ground objects in a specific context only
*** - To set impassible locations for objects in the sky layer
*** ***************************************************************************/
class VirtualSprite {
public:
	/** \brief A bit-mask for the sprite's draw orientation and direction vector.
	*** This member determines both where to move the sprite (8 directions) and
	*** which way the sprite is facing (4 directions). See the Sprite Directions
	*** series of constants for the values that this member may be set to.
	**/
	uint16 direction;

	//! \brief The speed at which the sprite moves around the map.
	float movement_speed;

	/** \brief Set to true when the sprite is currently moving.
	*** \note This does not necessarily mean that the sprite actually is moving, but rather that
	*** the sprite is <i>trying</i> to move in a certain direction.
	**/
	bool moving;

	//! \brief Set to true when the sprite is running rather than just walking
	bool is_running;

	//! \brief Set to selected in the editor
	bool is_selected;

	/** \brief When set to true, indicates that the object exists on the sky object layer (default = false).
	*** This member is necessary for collision detection purposes. When a sprite needs to detect
	*** if it has encountered a collision, that collision must be examined with other objects on
	*** the appropriate layer (the ground or sky layer).
	**/
	bool sky_object;

	//! \brief The name of the sprite, as seen by the player in the game.
	hoa_utils::ustring name;

	//! \brief A pointer to the face portrait of the sprite, as seen in dialogues and menus.
	hoa_video::StillImage* face_portrait;

	//! \brief Set to false if the sprite contains dialogue that has not been seen by the player
	//bool seen_all_dialogue;

	//! \brief True is sprite contains active dialogue.
	//bool has_active_dialogue;

	/** \brief An index to the actions vector, representing the current sprite action being performed.
	*** A negative value indicates that the sprite is taking no action. If the sprite has no entries
	*** in its actions vector, this member should remain negative, otherwise a segmentation fault
	*** will occur.
	**/
	int8 current_action;

	// TODO: change how forced action work
	//int8 forced_action;

	//! \brief A container for all of the actions this sprite performs.
	//std::vector<SpriteAction*> actions;

	/** \name Saved state attributes
	*** These attributes are used to save and load the state of a VirtualSprite
	**/
	//@{
	//! \brief This indicates if a state was saved or not.
	bool _saved;
	uint16 _saved_direction;
	float _saved_movement_speed;
	bool _saved_moving;
	hoa_utils::ustring _saved_name;
	//int8 _saved_current_action;
	//@}

	//! \brief This vector contains all the dialogues of the sprite
	//std::vector<MapDialogue*> dialogues;

	/** \brief An index to the dialogues vector, representing the current sprite dialogue to
	*** display when talked to by the player. A negative value indicates that the sprite has no dialogue.
	*** \note If the sprite has no entries in its dialogues vector, this member should remain negative,
	*** otherwise a segmentation fault will occur.
	**/
	//int16 _current_dialogue;

	//! \brief Indicates if the icon indicating that there is a dialogue available should be drawn or not.
	//bool _show_dialogue_icon;

	//! \brief Used to fade the dialogue icon according to distance 
	//hoa_video::Color _dialogue_icon_color;

	/** \brief An identification number for the object as it is represented in the map file.
	*** Player sprites are assigned object ids from 5000 and above. Technically this means that
	*** a map can have no more than 5000 objects that are not player sprites, but no map should
	*** need to contain that many objects in the first place. Objects with an ID less than zero
	*** are invalid.
	**/
	int16 object_id;

	/** \brief The map context that the object currently resides in.
	*** Context helps to determine where an object "resides". For example, inside of a house or
	*** outside of a house. The context member determines if the object should be drawn or not,
	*** since objects are only drawn if they are in the same context as the map's camera.
	*** Objects can only interact with one another if they both reside in the same context.
	***
	*** \note The default value for this member is -1. A negative context indicates that the
	*** object is invalid and it does not exist anywhere. Objects with a negative context are never
	*** drawn to the screen. A value equal to zero indicates that the object is "always in
	*** context", meaning that the object will be drawn regardless of the current context. An
	*** example of where this is useful is a bridge, which shouldn't simply disappear because the
	*** player walks inside a nearby home.
	**/
	uint32 context;

	/** \brief Coordinates for the object's origin/position.
	*** The origin of every map object is the bottom center point of the object. These
	*** origin coordinates are used to determine where the object is on the map as well
	*** as where the objects collision rectangle lies.
	***
	*** The position coordinates are described by an integer (position) and a float (offset).
	*** The position coordinates point to the map grid tile that the object currently occupies
	*** and may range from 0 to the number of columns or rows of grid tiles on the map. The
	*** offset member will always range from 0.0f and 1.0f to indicate the exact position of
	*** the object within that tile.
	**/
	//@{
	uint16 x_position, y_position;
	float x_offset, y_offset;
	//@}

	/** \brief The half-width and height of the image, in map grid coordinates.
	*** The half_width member is indeed just that: half the width of the object's image. We keep
	*** the half width rather than the full width because the origin of the object is its bottom
	*** center, and it is more convenient to store only half the sprite's width as a result.
	***
	*** \note These members assume that the object retains the same width and height regardless
	*** of the current animation or image being drawn. If the object's image changes size, the
	*** API user must remember to change these values accordingly.
	**/
	float img_half_width, img_height;

	/** \brief Determines the collision rectangle for the object.
	*** The collision area determines what portion of the map object may not be overlapped
	*** by other objects or unwalkable regions of the map. The x and y coordinates are
	*** relative to the origin, so an x value of 0.5f means that the collision rectangle
	*** extends the length of 1/2 of a tile from the origin on both sides, and a y value
	*** of 1.0f means that the collision area exists from the origin to 1 tile's length
	*** above.
	***
	*** \note These members should always be positive. Setting these members to zero does *not*
	*** eliminate collision detection for the object, and therefore they should usually never
	*** be zero.
	**/
	float coll_half_width, coll_height;

	//! \brief When set to false, the Update() function will do nothing (default = true).
	bool updatable;

	//! \brief When set to false, the Draw() function will do nothing (default = true).
	bool visible;

	/** \brief When set to true, the object will not be examined for collision detection (default = false).
	*** Setting this member to true really has two effects. First, the object may exist anywhere on
	*** the map, including where the collision rectangles of other objects are located. Second, the
	*** object is ignored when other objects are performing their collision detection. This property
	*** is useful for virtual objects or objects with an image but no "physical form" (i.e. ghosts
	*** that other sprites may walk through). Note that while this member is set to true, the object's
	*** collision rectangle members are ignored.
	**/
	bool no_collision;

	/** \brief When set to true, objects in the ground object layer will be drawn after the pass objects
	*** \note This member is only checked for objects that exist in the ground layer. It has no meaning
	*** for objects in the pass or sky layers.
	**/
	bool draw_on_second_pass;

	// -------------------- Public methods

	VirtualSprite();

	~VirtualSprite();

	//! \brief Updates the virtual object's position if it is moving, otherwise does nothing.
	virtual void Update();

	//! \brief Draws a dialogue icon over the virtual sprite if it has to.
	virtual void Draw();

	//! \brief Draws the selection image
	virtual void DrawSelection()=0;

	/** \name Lua Access Functions
	*** These functions are specifically written for Lua binding, to enable Lua to access the
	*** members of this class.
	**/
	//@{
	/** \note This method takes into account the current direction when setting the new direction
	*** in the case of diagonal movement. For example, if the sprite is currently facing north
	*** and this function indicates that the sprite should move northwest, it will face north
	*** during the northwest movement.
	**/
	void SetDirection(uint16 dir);

	void SetMovementSpeed(float speed)
		{ movement_speed = speed; }

	uint16 GetDirection() const
		{ return direction; }

	float GetMovementSpeed() const
		{ return movement_speed; }

	void SetFacePortrait(std::string pn);
	//@}

	/** \brief This method will save the state of a sprite.
	*** Attributes saved: direction, speed, moving state, name, current action.
	**/
	virtual void SaveState();

	/** \brief This method will load the saved state of a sprite.
	*** Attributes loaded: direction, speed, moving state, name, current action.
	*** \return false if there was no saved state, true otherwise.
	**/
	virtual bool LoadState();

	//! \brief Examines all dialogue owned by the sprite and sets the appropriate value of VirtualSprite#seen_all_dialogue
	//void UpdateSeenDialogue();

	//! \brief Examines all dialogue owned by the sprite and sets the appropriate value of VirtualSprite#has_active_dialogue
	//void UpdateActiveDialogue();

	/** \name Dialogue control methods
	*** These methods are used to add and control which dialogue should the sprite speak.
	**/
	//@{
	//void AddDialogue(MapDialogue* md);

	//bool HasDialogue() const
		//{ if(dialogues.size() > 0) return has_active_dialogue; else return false; }

	//MapDialogue* GetCurrentDialogue() const
		//{ return dialogues[_current_dialogue]; }

	//void SetDialogue(const int16 dialogue)
		//{ if (static_cast<uint16>(dialogue) >= dialogues.size()) return; else _current_dialogue = dialogue; }

	//void NextDialogue()
		//{ do { _current_dialogue++; if (static_cast<uint16>(_current_dialogue) >= dialogues.size()) _current_dialogue = 0; }
		//	while (dialogues[_current_dialogue]->IsActive() == false); }

	//int16 GetNumDialogues() const
		//{ return dialogues.size(); }

	//void ShowDialogueIcon(bool state)
		//{ _show_dialogue_icon = state; }

	//bool IsShowingDialogueIcon() const
		//{ return _show_dialogue_icon; }
	//@}

	/** \brief Adds a new action for the sprite to process onto the end of the sprite's action list
	*** \param act A pointer to the instantiated SpriteAction object to use
	**/
	//void AddAction(SpriteAction* act)
		//{ act->SetSprite(this); actions.push_back(act); }

	/** \brief This static class function returns the opposite direction of the direction given in parameter.
	*** \note This is mostly used as an helper function to make sprites face each other.
	**/
	static uint16 CalculateOppositeDirection(const uint16 direction);

	/** \brief Computes the full floating-point location coordinates of the object
	*** \return The full x or y coordinate location of the object
	***
	*** Since an object's position is stored as an integer component and an offset component, this
	*** method simply returns a single floating point value representing the full x and y positions
	*** of the object in a single variable.
	**/
	//@{
	float ComputeXLocation() const
		{ return (static_cast<float>(x_position) + x_offset); }

	float ComputeYLocation() const
		{ return (static_cast<float>(y_position) + y_offset); }

	float ComputeDrawXLocation() const
		{ return (static_cast<float>(x_position) - img_half_width) / X_POS_FACTOR + x_offset; }

	float ComputeDrawYLocation() const		
		{ return (static_cast<float>(y_position) - img_height) / Y_POS_FACTOR + y_offset; }

	QRectF ComputeHoverArea() const
		{ return QRectF(ComputeDrawXLocation(), ComputeDrawYLocation(), img_half_width, img_height/2); }

	bool IsInHoverArea(float x, float y) const
	{ return ComputeHoverArea().contains( QPointF(x,y) ); }
	//@}

	/** \brief Sets the sprite's direction to a random value
	*** This function is used mostly for the ActionRandomMove class.
	**/
	void SetRandomDirection();

	void SetObjectID(int16 id = 0)
		{ object_id = id; }

	void SetContext(uint32 ctxt)
		{ context = ctxt; }

	void SetXPosition(uint16 x, float offset)
		{ x_position = x; x_offset = offset; }

	void SetYPosition(uint16 y, float offset)
		{ y_position = y; y_offset = offset; }

	void SetImgHalfWidth(float width)
		{ img_half_width = width; }

	void SetImgHeight(float height)
		{ img_height = height; }

	void SetCollHalfWidth(float collision)
		{ coll_half_width = collision; }

	void SetCollHeight(float collision)
		{ coll_height = collision; }

	void SetUpdatable(bool update)
		{ updatable = update; }

	void SetVisible(bool vis)
		{ visible = vis; }

	void SetNoCollision(bool coll)
		{ no_collision = coll; }

	void SetDrawOnSecondPass(bool pass)
		{ draw_on_second_pass = pass; }

	int16 GetObjectID() const
		{ return object_id; }

	uint32 GetContext() const
		{ return context; }

	void GetXPosition(uint16 &x, float &offset) const
		{ x = x_position; offset = x_offset; }

	void GetYPosition(uint16 &y, float &offset) const
		{ y = y_position; offset = y_offset; }

	float GetImgHalfWidth() const
		{ return img_half_width; }

	float GetImgHeight() const
		{ return img_height; }

	float GetCollHalfWidth() const
		{ return coll_half_width; }

	float GetCollHeight() const
		{ return coll_height; }

	bool IsUpdatable() const
		{ return updatable; }

	bool IsVisible() const
		{ return visible; }

	bool IsNoCollision() const
		{ return no_collision; }

	bool IsDrawOnSecondPass() const
		{ return draw_on_second_pass; }

	uint8 GetType() const
		{ return _object_type; }

protected:
	//! \brief This holds the the type of sprite this is.
	uint8 _object_type;

	//! \brief Selected image for sprite has been selected
	hoa_video::StillImage _selected_image;

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
	//! \brief Holds the previous value of VirtualSprite#moving from the last call to MapSprite#Update().
	bool was_moving;

	//! \brief Set to true if the sprite has running animations loaded
	bool has_running_anim;

	/** \brief The sound that will play when the sprite walks.
	*** This member references the MapMode#_map_sounds vector as the sound to play. If this member
	*** is less than zero, no sound is played when the object is walking.
	**/
	int8 walk_sound;

	//! \brief The index to the animations vector containing the current sprite image to display
	uint8 current_animation;

	/** \brief A vector containing all the sprite's various animations.
	*** The first four entries in this vector are the walking animation frames.
	*** They are ordered from index 0 to 3 as: down, up, left, right. Additional
	*** animations may follow.
	**/
	std::vector<hoa_video::AnimatedImage> animations;

	/** \name Saved state attributes
	*** These attributes are used to save and load the state of a VirtualSprite
	**/
	//@{
	//bool _saved_was_moving;
	//int8 _saved_walk_sound;
	//uint8 _saved_current_animation;
	//@}

	// -------------------------------- Methods --------------------------------

	MapSprite();

	~MapSprite();

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

	//! \brief Updates the sprite's position and state.
	virtual void Update();

	//! \brief Draws the sprite frame in the appropriate position on the screen, if it is visible.
	virtual void Draw();

	//! \brief Draws the selection image
	virtual void DrawSelection();

	/** \name Lua Access Functions
	*** These functions are specifically written for Lua binding, to enable Lua to access the
	*** members of this class.
	**/
	//@{
	void SetName(std::string na)
		{ name = hoa_utils::MakeUnicodeString(na); }

	void SetWalkSound(int8 sound)
		{ walk_sound = sound; }

	void SetCurrentAnimation(uint8 anim)
		{ current_animation = anim; }

	int8 GetWalkSound() const
		{ return walk_sound; }

	uint8 GetCurrentAnimation() const
		{ return current_animation; }
	//@}

	/** \brief This method will save the state of a sprite.
	*** Attributes saved: direction, speed, moving state, name, current action,
	*** current animation, current walk sound.
	**/
	//virtual void SaveState();

	/** \brief This method will load the saved state of a sprite.
	*** Attributes loaded: direction, speed, moving state, name, current action,
	*** current animation, current walk sound.
	*** \return false if there was no saved state, true otherwise.
	**/
	//virtual bool LoadState();
}; // class MapSprite : public VirtualSprite

} // namespace hoa_map

#endif // __SPRITES_HEADER__
