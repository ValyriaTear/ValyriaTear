///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_objects.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode objects.
*** *****************************************************************************/

#ifndef __MAP_OBJECTS_HEADER__
#define __MAP_OBJECTS_HEADER__

#include "modes/map/map_treasure.h"

namespace hoa_map {

namespace private_map {

/** ****************************************************************************
*** \brief Abstract class that represents objects on a map
***
*** A map object can be anything from a sprite to a tree to a house. To state
*** it simply, a map object is a map image that is not tiled and need not be fixed
*** in place. Map objects are drawn in one of three layers: ground, pass, and sky
*** object layers.

*** All map objects have both a collision rectangle and an image rectangle.
*** The collision rectangle indicates what parts of the object may not overlap
*** with other collision rectangles and unwalkable sections of the map. The image
*** rectangle determines the size of the object as it is visible on the screen.
*** The collision rectangle and image rectangles do not need to be the same size.
*** Typically the collision rectangle is smaller than the image rectangle. It is
*** also possible to disable both rectangles via special properties that can be
*** enabled in this class. This would prevent the object from being a factor in
*** collision detection and/or it would never be drawn to the screen.
***
*** State information about map objects may need to be retained upon leaving a
*** map. For example, a treasure (which is a type of map object) needs to know
*** whether or not the player has retrieved its contents already so that they
*** can not be gained a second time. This data is stored in the saved game file
*** so that even when the player exits the game, the state information can be
*** retrieved when the application starts again later.
***
*** \note It is advised not to attempt to make map objects with dynamic sizes (i.e.
*** the various image frames that compose the object should all be the same size).
*** In theory, dynamically sized objects are feasible to implement in maps, but
*** they are much easier to be subject to bugs and other issues.
*** ***************************************************************************/
class MapObject {
public:
	MapObject();

	virtual ~MapObject()
	{}

	/** \brief An identification number for the object as it is represented in the map file.
	*** Player sprites are assigned object IDs from 5000 and above. Technically this means that
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
	*** \note The default value for this member is the base context (context 01).
	**/
	MAP_CONTEXT context;

	/** \brief Coordinates for the object's origin/position.
	*** The origin of every map object is the bottom center point of the object. These
	*** origin coordinates are used to determine where the object is on the map as well
	*** as where the objects collision rectangle lies.
	***
	*** The position coordinates point to the map grid tile that the object currently occupies
	*** and may range from 0 to the number of columns or rows of grid tiles on the map.
	**/
	//@{
	MapPosition position;
	//@}

	/** \brief The half-width and height of the image, in map grid coordinates.
	*** The half_width member is indeed just that: half the width of the object's image. We keep
	*** the half width rather than the full width because the origin of the object is its bottom
	*** center, and it is more convenient to store only half the sprite's width.
	***
	*** \note These members assume that the object retains the same width and height regardless
	*** of the current animation frame or image being drawn. If the object's image changes size
	*** for any reason, the programmer must remember to change these values accordingly.
	**/
	float img_half_width, img_height;

	/** \brief Determines the collision rectangle for the object.
	*** The collision area determines what portion of the map object may not be overlapped
	*** by other objects or unwalkable regions of the map. The x and y coordinates are
	*** relative to the origin, so an x value of 0.5f means that the collision rectangle
	*** extends the length of 1/2 of a grid element from the origin on both sides, and a y value
	*** of 1.0f means that the collision area exists from the origin to one grid element above.
	***
	*** \note These members should always be positive and non-zero. Setting these members to
	*** zero does <b>not</b> eliminate collision detection for the object.
	**/
	float coll_half_width, coll_height;

	//! \name Object Properties
	//@{
	//! \brief When false, the Update() function will do nothing (default == true).
	bool updatable;

	//! \brief When false, the Draw() function will do nothing (default == true).
	bool visible;

	//! \brief The collision mask indiacting what the object will collide with. (i.e.: walls + objects, nothing, ...)
	//! \NOTE: COLLISION TYPE used as bitmask
	uint32 collision_mask;

	/** \brief When true, indicates that the object exists on the sky object layer (default == false).
	*** This member is necessary for collision detection purposes. When a sprite needs to detect
	*** if it has encountered a collision, that collision must be examined with other objects on
	*** the appropriate layer (the ground/pass layers or the sky layer).
	**/
	bool sky_object;

	/** \brief When true, objects in the ground object layer will be drawn after the pass objects
	*** This member is only checked for objects that exist in the ground layer. It has no meaning
	*** for objects in the pass or sky layers. Its purpose is so that objects (such as a bridge)
	*** in the pass layer can be both walked over and walked under by sprites in the ground layer.
	**/
	bool draw_on_second_pass;
	//@}

	// ---------- Methods

	/** \brief Updates the state of an object.
	*** Many map objects may not actually have a use for this function. For example, animated objects
	*** like a tree will automatically have their frames updated by the video engine in the draw
	*** function. So it is the case that the implementation of this function in derived classes may
	*** simply do nothing.
	**/
	virtual void Update() = 0;

	/** \brief Draws the object to the frame buffer.
	*** Objects are drawn differently depending on what type of object they are and what their current
	*** state is. Note that calling this function does not guarantee that the object will be drawn.
	*** Many implementations of this function in the derived classes first call the ShouldDraw() method
	*** to determine if the object should be drawn at all.
	**/
	virtual void Draw() = 0;

	/** \brief Determines if an object should be drawn to the screen.
	*** \return True if the object should be drawn.
	*** \note This function also moves the draw cursor to the proper position if the object should be drawn
	***
	*** This method performs the common drawing operations of identifying whether or not the object
	*** is visible on the screen and moving the drawing cursor to its location. The children classes
	*** of this class may choose to make use of it (or not).
	**/
	bool ShouldDraw();
	//@}

	//! \brief Retrieves the object type identifier
	MAP_OBJECT_TYPE GetObjectType() const
		{ return _object_type; }

	/** \brief Returns the collision rectangle for the current object
	**/
	MapRectangle GetCollisionRectangle() const;

	/** \brief Returns the collision rectangle for the current object for the given position
	*** \return rect A MapRectangle object storing the collision rectangle data
	*** using the given position.
	**/
	MapRectangle GetCollisionRectangle(float x, float y) const;

	/** \brief Returns the image rectangle for the current object
	*** \param rect A MapRectangle object storing the image rectangle data
	**/
	virtual MapRectangle GetImageRectangle() const;

	/** \brief Restores the saved state of the object
	*** This state data is retained in the saved game file. When any map object is created and added
	*** to the map, this function is called to load any stored state data that there may be. Notice
	*** that the default implementation of this function does nothing.
	**/
	virtual void RestoreSaved()
	{}

	/** \name Lua Access Functions
	*** These functions are specifically written to enable Lua to access the members of this class.
	*** C++ code may also choose to use these functions, although all of the members here are public
	*** so it is not mandatory to do so.
	**/
	//@{
	void SetObjectID(int16 id = 0)
	{ object_id = id; }

	void SetContext(MAP_CONTEXT ctxt)
	{ context = ctxt; }

	void SetPosition(float x, float y)
	{ position.x = x; position.y = y; }

	void SetXPosition(float x)
	{ position.x = x; }

	void SetYPosition(float y)
	{ position.y = y; }

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

	// Use a set of COLLISION_TYPE bitmask values
	void SetCollisionMask(uint32 collision_types)
	{ collision_mask = collision_types; }

	void SetDrawOnSecondPass(bool pass)
	{ draw_on_second_pass = pass; }

	int16 GetObjectID() const
	{ return object_id; }

	MAP_CONTEXT GetContext() const
	{ return context; }

	MapPosition GetPosition() const
	{ return position; }

	float GetXPosition() const
	{ return position.x; }

	float GetYPosition() const
	{ return position.y; }

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

	uint32 GetCollisionMask() const
	{ return collision_mask; }

	bool IsDrawOnSecondPass() const
	{ return draw_on_second_pass; }

	MAP_OBJECT_TYPE GetType() const
	{ return _object_type; }

	/** \brief Play the corresponding emote animation set in the emotes.lua file
	*** \see LoadEmotes() in the GameGlobal class.
	**/
	void Emote(const std::string& emote_name, hoa_map::private_map::ANIM_DIRECTIONS dir = hoa_map::private_map::ANIM_SOUTH);

	//! \brief Indicates whether the given map object is using an emote animation.
	bool HasEmote() const
	{ return (_emote_animation); }
	//@}

protected:
	//! \brief This is used to identify the type of map object for inheriting classes.
	MAP_OBJECT_TYPE _object_type;

	//! \brief the emote animation to play
	hoa_video::AnimatedImage *_emote_animation;

	//! \brief The emote animation drawing offset (depending on the map object direction)
	float _emote_offset_x;
	float _emote_offset_y;

	//! \brief the time the emote animatio will last in milliseconds,
	int32 _emote_time;

	//! \brief Takes care of updating the emote animation and state.
	void _UpdateEmote();

	//! \brief Takes care of drawing the emote animation.
	void _DrawEmote();
}; // class MapObject


/** \brief This is a predicate used to sort MapObjects in correct draw order
*** \return True if the MapObject pointed by a should be drawn behind MapObject pointed by b
*** \note A simple '<' operator cannot be used with the sorting algorithm because it is sorting pointers.
**/
struct MapObject_Ptr_Less {
	bool operator()(const MapObject* a, const MapObject* b) {
		return (a->position.y) < (b->position.y);
	}
};


/** ****************************************************************************
*** \brief Represents visible objects on the map that have no motion.
***
*** This class represents both still image and animated objects. These objects
*** are usually fixed in place and do not change their position.
***
*** \note If the object does not have any animated images, set the 'updatable'
*** member of the base class to false. Forgetting to do this will do no harm, but
*** it will make it heavier.
*** ***************************************************************************/
class PhysicalObject : public MapObject {
public:
	PhysicalObject();

	~PhysicalObject();

	/** \brief The index to the animations vector that contains the current image to display
	*** When modifying this member, take care not to exceed the bounds of the animations vector
	**/
	uint8 current_animation;

	/** \brief A vector containing all the object's animations.
	*** These need not be actual animations. If you just want a still image, add only a single
	*** frame to the animation. Usually only need a single still image or animation will be
	*** needed, but a vector is used here in case others are needed.
	**/
	std::vector<hoa_video::AnimatedImage> animations;

	//! \brief Updates the object's current animation.
	virtual void Update();

	//! \brief Draws the object to the screen, if it is visible.
	virtual void Draw();

	/** \name Lua Access Functions
	*** These functions are specifically to enable Lua to access the members of this class.
	**/
	//@{
	/** \brief Sets a new animation using the animation filename provided
	*** \param animation_filename The name of the animation file to use for the animation
	*** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
	**/
	int32 AddAnimation(const std::string& animation_filename);

	/** \brief Sets a new still animation using the image filename provided
	*** \param image_filename The name of the image file to use for the animation
	*** \return The animation id that can later be used with SetCurrentAnimation() or -1 if invalid
	**/
	int32 AddStillFrame(const std::string& image_filename);

	void AddAnimation(hoa_video::AnimatedImage new_img)
	{ animations.push_back(new_img); }

	void SetCurrentAnimation(uint32 animation_id);

	void SetAnimationProgress(uint32 progress)
	{ animations[current_animation].SetTimeProgress(progress); }

	uint32 GetCurrentAnimation() const
	{ return current_animation; }
	//@}
}; // class PhysicalObject : public MapObject

/** ****************************************************************************
*** \brief Represents particle object on the map
*** ***************************************************************************/
class ParticleObject : public MapObject {
public:
	ParticleObject(const std::string& filename, float x, float y, MAP_CONTEXT map_context);

	~ParticleObject();

	//! \brief Updates the object's current animation.
	//! \note the actual image resources is handled by the main map object.
	void Update();

	//! \brief Draws the object to the screen, if it is visible.
	//! \note the actual image resources is handled by the main map object.
	void Draw();

	//! \brief Start or restart the particle effect
	void Stop();

	//! \brief Stop the particle effect
	bool Start();

private:
	//! \brief A reference to the current map save animation.
	hoa_mode_manager::ParticleEffect *_particle_effect;

	//@}
}; // class ParticleObject : public MapObject

/** ****************************************************************************
*** \brief Represents save point on the map
*** ***************************************************************************/
class SavePoint : public MapObject {
public:
	SavePoint(float x, float y, MAP_CONTEXT map_context);

	~SavePoint()
	{}

	//! \brief Updates the object's current animation.
	//! \note the actual image resources is handled by the main map object.
	void Update();

	//! \brief Draws the object to the screen, if it is visible.
	//! \note the actual image resources is handled by the main map object.
	void Draw();

	//! \brief Tells whether a character is in or not, and setup the animation
	//! accordingly.
	void SetActive(bool active);

private:
	//! \brief A reference to the current map save animation.
	std::vector<hoa_video::AnimatedImage>* _animations;

	//! \brief The corresponding particle object for active/inactive save points pointers
	// Note that those pointers are managed by the object supervisor
	ParticleObject *_active_particle_object;
	ParticleObject *_inactive_particle_object;

	//! \brief Tells whether the save has become active
	bool _save_active;
	//@}
}; // class SavePoint : public MapObject


/** ****************************************************************************
*** \brief Represents a halo (source of light) on the map
*** ***************************************************************************/
class Halo : public MapObject {
public:
    //! \brief setup a halo on the map, using the given animation file.
	Halo(const std::string& filename, float x, float y,
		 const hoa_video::Color& color, MAP_CONTEXT map_context);

	~Halo()
	{}

	//! \brief Updates the object's current animation.
	//! \note the actual image resources is handled by the main map object.
	void Update();

	//! \brief Draws the object to the screen, if it is visible.
	//! \note the actual image resources is handled by the main map object.
	void Draw();


private:
	//! \brief A reference to the current map save animation.
	hoa_video::AnimatedImage _animation;

	//! The blending color of the halo
	hoa_video::Color _color;

	//@}
}; // class Halo : public MapObject

/** ****************************************************************************
*** \brief Represents a source of light on the map, changing its orientation
*** according to the camera view.
*** ***************************************************************************/
class Light : public MapObject {
public:
	//! \brief setup a halo on the map, using the given animation file.
	Light(const std::string& main_flare_filename,
		 const std::string& secondary_flare_filename,
		 float x, float y,
		 const hoa_video::Color& main_color, const hoa_video::Color& secondary_color,
		 MAP_CONTEXT map_context);

	~Light()
	{}

	//! \brief Updates the object's current animation and orientation
	//! \note the actual image resources is handled by the main map object.
	void Update();

	//! \brief Draws the object to the screen, if it is visible.
	//! \note the actual image resources is handled by the main map object.
	void Draw();

	/** \brief Returns the image rectangle for the current object
	*** \param rect A MapRectangle object storing the image rectangle data
	**/
	MapRectangle GetImageRectangle() const;
private:
	//! Updates the angle and distance from the camera viewpoint
	void _UpdateLightAngle();

	//! \brief A reference to the current light animation.
	hoa_video::AnimatedImage _main_animation;
	hoa_video::AnimatedImage _secondary_animation;

	//! The blending color of the light
	hoa_video::Color _main_color;
	hoa_video::Color _secondary_color;

	//! The blending color with dynamic alpha, for better rendering
	hoa_video::Color _main_color_alpha;
	hoa_video::Color _secondary_color_alpha;

	//! used to compute the flare lines equation.
	float _a, _b;
	//! Distance between the light and the camera viewpoint.
	float _distance;

	//! Random distance factor used to make the secondary flares appear at random places
	float _distance_factor_1;
	float _distance_factor_2;
	float _distance_factor_3;
	float _distance_factor_4;

	/** \brief Used for optimization, keeps the last center position.
	*** So that we update the distance and angle only when this position has changed.
	**/
	MapPosition _last_center_pos;

	//@}
}; // class Light : public MapObject


/** ****************************************************************************
*** \brief Represents an obtainable treasure on the map which the player may access
***
*** A treasure is a specific type of physical object, usually in the form of a
*** treasure chest. When the player accesses these treasures, the chest animates as
*** it is being opened and the treasure supervisor is initialized once the opening
*** animation is complete. Each treasure object on a map has a global event associated
*** with it to determine whether the treasure contents have already been retrieved by
*** the player.
***
*** Image files for treasures are single row multi images where the frame ordering
*** goes from closed, to opening, to open. This means each map treasure has exactly
*** three animations. The closed and open animations are usually single frame images.
***
*** To add contents to the treasure for this object, you will need to retreive the
*** pointer to the MapTreasure object via the GetTreasure() method, then add drunes
*** and/or objects (items/equipment/etc) to the MapTreasure.
***
*** \todo Add support for more treasure features, such as locked chests, chests which
*** trigger a battle, etc.
*** ***************************************************************************/
class TreasureObject : public PhysicalObject {
	//! \brief Constants representing the three types of animations for the treasure
	enum {
		TREASURE_CLOSED_ANIM   = 0,
		TREASURE_OPENING_ANIM  = 1,
		TREASURE_OPEN_ANIM     = 2
	};

public:
	/** \param treasure_name The name of the treasure. Used to store and load the treasure state.
	*** \param treasure The treasure object holding its content.
	*** \param closed_animation_file The animation file used to display the treasure when it is closed.
	*** \param opening_animation_file The animation file used to display the treasure when it is opening.
	*** \param open_animation_file The animation file used to display the treasure when it is open.
	**/
	TreasureObject(const std::string& treasure_name,
				const std::string& closed_animation_file,
				const std::string& opening_animation_file,
				const std::string& open_animation_file);

	~TreasureObject()
		{ delete _treasure; }

	std::string GetTreasureName() const
		{ return _treasure_name; }

	//! \brief Loads the state of the chest from the global event corresponding to the current map
	void LoadState();

	//! \brief Opens the treasure, which changes the active animation and initializes the treasure supervisor when the opening animation finishes.
	void Open();

	//! \brief Changes the current animation if it has finished looping
	void Update();

	//! \brief Retrieves a pointer to the MapTreasure object holding the treasure.
	MapTreasure* GetTreasure()
		{ return _treasure; }

	//! \brief Sets the number of drunes present in the chest's contents.
	void SetDrunes(uint32 amount)
	{ _treasure->SetDrunes(amount); }

	/** \brief Adds an object to the contents of the TreasureObject
	*** \param id The id of the GlobalObject to add
	*** \param quantity The number of the object to add (default == 1)
	*** \return True if the object was added successfully
	**/
	bool AddObject(uint32 id, uint32 quantity = 1);

private:
	//! \brief Stores the contents of the treasure which will be processed by the treasure supervisor
	MapTreasure* _treasure;

	//! \brief The treasure object name
	std::string _treasure_name;
}; // class TreasureObject : public PhysicalObject


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
class ObjectSupervisor {
	friend class hoa_map::MapMode;
	// TEMP: for allowing context zones to access all objects
	friend class hoa_map::private_map::ContextZone;
	friend void hoa_defs::BindModeCode();

public:
	ObjectSupervisor();

	~ObjectSupervisor();

	//! \brief Returns a unique ID integer for an object to use
	uint16 GenerateObjectID()
		{ return ++_last_id; }

	//! \brief Returns the number of objects stored by the supervisor, regardless of what layer they exist on
	uint32 GetNumberObjects() const
		{ return _all_objects.size(); }

	/** \brief Retrieves an object by its position in the _all_objects container
	*** \param index The index of the object to retrieve
	*** \return A pointer to the object at this index, or NULL if no object exists at the given index
	***
	*** \note It is uncommon to require the use of this function in a map. It exists for Lua to be able to access
	*** all available map objects even when the IDs of those objects are unknown.
	**/
	MapObject* GetObjectByIndex(uint32 index);

	/** \brief Retrieves a pointer to an object on this map
	*** \param object_id The id number of the object to retreive
	*** \return A pointer to the map object, or NULL if no object with that ID was found
	**/
	MapObject* GetObject(uint32 object_id);

	/** \brief Retrieves a pointer to a sprite on this map
	*** \param object_id The id number of the sprite to retreive
	*** \return A pointer to the sprite object, or NULL if the object was not found or was not a sprite type
	**/
	VirtualSprite* GetSprite(uint32 object_id);

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
	bool Load(hoa_script::ReadScriptDescriptor& map_file);

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
	void DrawSavePoints();
	void DrawGroundObjects(const bool second_pass);
	void DrawPassObjects();
	void DrawSkyObjects();
	void DrawLights();
	void DrawDialogIcons();
	//@}

	/** \brief Finds the nearest interactable map object within a certain distance of a sprite
	*** \param sprite The sprite who is trying to find its nearest object
	*** \param search_distance The maximum distance to search for an object from the sprite (default == 3.0f)
	*** \return A pointer to the nearest map object, or NULL if no such object was found.
	***
	*** An interactable object must be in the same context as the function argument is. For an object
	*** to be valid, it's collision rectangle must be no greater than the search distance (in units of
	*** collision grid elements) from the sprite's "calling" axis. For example, if the search distance was 3.0f
	*** and the sprite was facing downwards, this function draws an imaginary rectangle below the sprite of height
	*** 3.0f and a length equal to the length of the sprite. Any objects that have their collision rectangles intersect
	*** with any portion of this search area are put on a list of valid objects, and once this list has been fully
	*** constructed the nearest of these objects will be returned.
	**/
	private_map::MapObject* FindNearestInteractionObject(const private_map::VirtualSprite* sprite, float search_distance = 3.0f);

	/** \brief Determines if a map object's collision rectangle intersects with a specified map area
	*** \param rect A reference to the rectangular section of the map to do collision detection with
	*** \param obj A pointer to a map object
	*** \return True if the objects collide with one another
	*** \note This test is "absolute", and does not factor in things such as map contexts or whether or
	*** not the no_collision property is enabled on the MapObject.
	**/
	bool CheckObjectCollision(const MapRectangle& rect, const private_map::MapObject* const obj);

	/** \brief Determines if a specific map object occupies a specific element of the collision grid
	*** \param x The x axis position
	*** \param y The y axis position
	*** \param object The object to check for occupation of the grid element
	*** \return True if the grid element is occupied by the object
	***
	*** \todo Take into account the object/sprite's collision property and also add a parameter for map context
	**/
	bool IsPositionOccupiedByObject(float x, float y, MapObject* object);

	/** \brief Tells the collision type corresponding to an object type.
	*** \param obj A pointer to the map object to check
	*** \return The corresponding type of collision detected.
	**/
	COLLISION_TYPE GetCollisionFromObjectType(MapObject *obj) const;

	/** \brief Tells the collision type of a sprite when it is at the given position
	*** \param sprite A pointer to the map sprite to check
	*** \param x The collision point on the x axis
	*** \param y The collision point on the y axis
	*** \param coll_obj A pointer to the MapObject that the sprite has collided with, if any
	*** \return The type of collision detected, which may include NO_COLLISION
	*** if none was detected
	***
	*** This method is invoked by a map sprite who wishes to check for its own collision.
	*** \See COLLISION_TYPE for more information.
	**/
	COLLISION_TYPE DetectCollision(VirtualSprite* sprite, float x, float y,
								   MapObject** collision_object_ptr = NULL);

	/** \brief Finds a path from a sprite's current position to a destination
	*** \param sprite A pointer of the sprite to find the path for
	*** \param dest The destination coordinates
	*** \param path A vector of PathNode objects storing the path
	***
	*** This algorithm uses the A* algorithm to find a path from a source to a destination.
	*** This function ignores the position of all other objects and only concerns itself with
	*** which map grid elements are walkable.
	***
	*** \note If an error is detected or a path could not be found, the function will empty the path vector before returning
	**/
	Path FindPath(private_map::VirtualSprite* sprite, const MapPosition& destination);

	/** \brief Returns the pointer to the virtual focus.
	**/
	private_map::VirtualSprite* VirtualFocus()
	{ return _virtual_focus; }

	/** Tells the object supervisor that the given sprite pointer
	*** is the party member object.
	*** This later permits to refresh the sprite shown based on the battle
	*** formation front party member.
	**/
	void SetPartyMemberVisibleSprite(private_map::MapSprite *sprite)
	{ _visible_party_member = sprite; }

	/** Updates the party member sprite based on the first active party member
	*** given by the global Manager.
	**/
	void ReloadVisiblePartyMember();

	//! \brief Tells whether the collision coords are valid.
	bool IsWithinMapBounds(float x, float y) const;

	//! \brief Tells whether the sprite has got valid collision coordinates.
	bool IsWithinMapBounds(VirtualSprite *sprite) const;

	//! \brief Draw the collision rectangles. Used for debugging purpose.
	void DrawCollisionArea(const MapFrame* frame);

private:
	//! \brief Returns the nearest save point. Used by FindNearestObject.
	private_map::MapObject* _FindNearestSavePoint(const VirtualSprite* sprite);

	//! \brief Updates save points animation and active state.
	void _UpdateSavePoints();

	//! \brief Debug: Draws the map zones in orange
	void _DrawMapZones();

	/** \brief The number of rows and columns in the collision gride
	*** The number of collision grid rows and columns is always equal to twice
	*** that of the number of rows and columns of tiles (stored in the TileManager).
	**/
	uint16 _num_grid_x_axis, _num_grid_y_axis;

	//! \brief Holds the most recently generated object ID number
	uint16 _last_id;

	/** \brief A "virtual sprite" that can serve as a focus point for the camera.
	*** This sprite is not visible to the player nor does it have any collision
	*** detection properties. Usually, the camera focuses on the player's sprite
	*** rather than this object, but it is useful for scripted sequences and other
	*** things.
	**/
	private_map::VirtualSprite *_virtual_focus;

	/** \brief The party member object is used to keep in memory the active member
	*** seen on map. This is later useful in "dungeon" maps for instance, where
	*** the party member in front of the battle formation is the one shown on map.
	*** Do not create or delete it in the code, this is just a reference.
	**/
	private_map::MapSprite *_visible_party_member;

	/** \brief A 2D vector indicating which grid element on the map sprites may be occupied by objects.
	*** Each bit of each element in this grid corresponds to a context. So all together this entire grid
	*** stores the collision information for all 32 possible map contexts.
	*** \Note A position in this member is stored like this:
	*** _collision_grid[y][x]
	**/
	std::vector<std::vector<uint32> > _collision_grid;

	/** \brief A map containing pointers to all of the sprites on a map.
	*** This map does not include a pointer to the _virtual_focus object. The
	*** sprite's unique identifier integer is used as the map key.
	**/
	std::map<uint16, MapObject*> _all_objects;

	/** \brief A container for all of the map objects located on the ground layer.
	*** The ground object layer is where most objects and sprites exist in a typical map.
	**/
	std::vector<MapObject*> _ground_objects;

	//! \brief A container for all of the save points, quite similar as the ground objects container.
	//! \note Save points are not registered in _all_objects.
	std::vector<SavePoint*> _save_points;

	/** \brief A container for all of the map objects located on the pass layer.
	*** The pass object layer is named so because objects on this layer can both be
	*** walked under or above by objects in the ground object layer. A good example
	*** of an object that would typically go on this layer would be a bridge. This
	*** layer usually has very few objects for the map.
	**/
	std::vector<MapObject*> _pass_objects;

	/** \brief A container for all of the map objects located on the sky layer.
	*** The sky object layer contains the last series of elements that are drawn on
	*** a map. These objects exist high in the sky above all other tiles and objects.
	*** Translucent clouds can make good use of this object layer, for instance.
	**/
	std::vector<MapObject*> _sky_objects;

	//! \brief Containers for all of the map source of light, quite similar as the ground objects container.
	//! \note Halos and lights are not registered in _all_objects.
	std::vector<Halo*> _halos;
	std::vector<Light*> _lights;

	//! \brief Container for all zones used in this map
	std::vector<MapZone*> _zones;

	/** \brief Container for all resident zones used in this map
	***
	*** Resident zones (and classes derived from resident zones) are stored in an additional container because
	*** they need to continually check whether sprites have entered them. This is done in the Update() function
	*** of this class.
	**/
	std::vector<ResidentZone*> _resident_zones;
}; // class ObjectSupervisor

} // namespace private_map

} // namespace hoa_map

#endif // __MAP_OBJECTS_HEADER__
