////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for the global game manager
***
*** This file contains the GameGlobal class, which is used to manage all data
*** that is shared "globally" by the various game modes. For example, it
*** contains the current characters in the party, the party's inventory, etc.
*** The definition of characters, items, and other related global data are
*** implemented in the other global header files (e.g. global_actors.h). All
*** of these global files share the same hoa_global namespace.
*** ***************************************************************************/

#ifndef __GLOBAL_HEADER__
#define __GLOBAL_HEADER__

#include "defs.h"
#include "utils.h"
#include "script.h"

#include "global_actors.h"
#include "global_effects.h"
#include "global_objects.h"
#include "global_skills.h"
#include "global_utils.h"

//! \brief All calls to global code are wrapped inside this namespace.
namespace hoa_global {

//! \brief The singleton pointer responsible for the management of global game data.
extern GameGlobal* GlobalManager;

//! \brief Determines whether the code in the hoa_global namespace should print debug statements or not.
extern bool GLOBAL_DEBUG;

/** ****************************************************************************
*** \brief A container that manages the occurences of several related game events
***
*** Events in Allacrost are nothing more than a string-integer pair. The string
*** represents the name of the event while the integer takes on various meanings
*** about the event. One example of an event could be if the player has already
*** seen a certain piece of dialogue, and the integer would be set to zero or
*** non-zero to emulate a boolean value. Another example could be whether the
*** player previous chose option A, B, C, or D when presented with a list of
*** possible actions to take, in which the integer value would represent the
*** option taken.
***
*** Because we want to continually look-up whether or not an event has occured,
*** it is not efficient to store all game events in a single container (the
*** larger the number of events, the longer the event search time). Instead,
*** this class collectively represents a group of related events. A typical
*** event group could represent all of the events that occured on a particular
*** map, for instance.
***
*** \note Other parts of the code should not have a need to construct objects of
*** this class. The GameGlobal class maintains a container of GlobalEventGroup
*** objects and provides methods to allow the creation, modification, and
*** retrieval of these objects.
*** ***************************************************************************/
class GlobalEventGroup {
public:
	//! \param group_name The name of the group to create (this can not be changed)
	GlobalEventGroup(const std::string& group_name) :
		_group_name(group_name) {}

	~GlobalEventGroup() {}

	/** \brief Queries whether or not an event of a given name exists in the group
	*** \param event_name The name of the event to check for
	*** \return True if the event name was found in the group, false if it was not
	**/
	bool DoesEventExist(const std::string& event_name)
		{ if (_events.find(event_name) != _events.end()) return true; else return false; }

	/** \brief Adds a new event to the group
	*** \param event_name The name of the event to add
	*** \param event_value The value of the event to add (default value is zero)
	*** \note If an event by the given name already exists, a warning will be printed and no addition
	*** or modification of any kind will take place
	**/
	void AddNewEvent(const std::string& event_name, int32 event_value = 0);

	/** \brief Retrieves the value of a specific event in the group
	*** \param event_name The name of the event to retrieve
	*** \return The value of the event, or GLOBAL_BAD_EVENT if there is no event corresponding to
	*** the requested event named
	**/
	int32 GetEvent(const std::string& event_name);

	/** \brief Sets the value for an existing event
	*** \param event_name The name of the event whose value should be changed
	*** \param event_value The value to set for the event
	*** \note If the event by the given name is not found, a warning will be printed and no change will be made
	**/
	void SetEvent(const std::string& event_name, int32 event_value);

	//! \brief Returns the number of events currently stored within the group
	uint32 GetNumberEvents() const
		{ return _events.size(); }

	//! \brief Returns a copy of the name of this group
	std::string GetGroupName() const
		{ return _group_name; }

	//! \brief Returns an immutable reference to the private _events container
	const std::map<std::string, int32>& GetEvents() const
		{ return _events; }

private:
	//! \brief The name given to this group of events
	std::string _group_name;

	/** \brief The map container for all the events in the group
	*** The string is the name of the event, which is unique within the group. The integer value
	*** represents the event's state and can take on multiple meanings depending on the context
	*** of this specific event.
	**/
	std::map<std::string, int32> _events;
}; // class GlobalEventGroup


/** ****************************************************************************
*** \brief Retains all the state information about the active game
***
*** This class is a resource manager for the current state of the game that is
*** being played. It retains all of the characters in the player's party, the
*** party's inventory, game events, etc. Nearly every game mode will need to
*** interact with this class in some form or another, whether it is to retrieve a
*** specific set of data or t
***
*** \note This class is a singleton, even though it is technically not an engine
*** manager class. There can only be one game instance that the player is playing
*** at any given time.
*** ***************************************************************************/
class GameGlobal : public hoa_utils::Singleton<GameGlobal> {
	friend class hoa_utils::Singleton<GameGlobal>;

public:
	~GameGlobal();

	bool SingletonInitialize();

	/** \brief Deletes all data stored within the GameGlobal class object
	*** This function is meant to be called when the user quits the current game instance
	*** and returns to the boot screen. It will delete all characters, inventory, and other
	*** data relevant to the current game.
	**/
	void ClearAllData();

	//! \name Character Functions
	//@{
	/** \brief Adds a new character to the party with its initial settings
	*** \param id The ID number of the character to add to the party.
	***
	*** Only use this function for when you wish the character to be constructed using
	*** its initial stats, equipment, and skills. Otherwise, you should construct the
	*** GlobalCharacter externally and invoke the other AddCharacter function with a
	*** pointer to it.
	***
	*** \note If the number of characters is less than four when this function is called,
	*** the new character will automatically be added to the active party.
	**/
	void AddCharacter(uint32 id);

	/** \brief Adds a new pre-initialized character to the party
	*** \param ch A pointer to the initialized GlobalCharacter object to add
	***
	*** The GlobalCharacter argument must be created -and- properly initalized (stats
	*** members all set, equipment added, skills added) prior to making this call.
	*** Adding an uninitialized character will likely result in a segmentation fault
	*** or other run-time error somewhere down the road.
	***
	*** \note If the number of characters is less than four when this function is called,
	*** the new character will automatically be added to the active party.
	**/
	void AddCharacter(GlobalCharacter* ch);

	/** \brief Removes a character from the party.
	*** \param id The ID number of the character to remove from the party.
	**/
	void RemoveCharacter(uint32 id);

	/** \brief Returns a pointer to a character currently in the party.
	*** \param id The ID number of the character to retrieve.
	*** \return A pointer to the character, or NULL if the character was not found.
	***/
	GlobalCharacter* GetCharacter(uint32 id);

	/** \brief Checks whether or not a character is in the party
	*** \param id The id of the character to check for
	*** \return True if the character was found to be in the party, or false if they were not found.
	**/
	bool IsCharacterInParty(uint32 id)
		{ if (_characters.find(id) != _characters.end()) return true; else return false; }
	//@}

	//! \name Inventory Methods
	//@{
	/** \brief Adds a new object to the inventory
	*** \param obj_id The identifier value of the object to add
	*** \param obj_count The number of instances of the object to add (default == 1)
	*** If the item already exists in the inventory, then instead the GlobalObject#_count member is used to
	*** increment the count of the stored item.
	**/
	void AddToInventory(uint32 obj_id, uint32 obj_count = 1);

	/** \brief Adds a new object to the inventory
	*** \param object A pointer to the pre-created GlobalObject-type class to add
	***
	*** Once you call this function, GameGlobal assumes it is now responsible for memory management of this
	*** object. Therefore, you should <b>never</b> attempt to reference the argument pointer after it is
	*** passed to this function, because it may very well now point to an invalid location in memory. You
	*** should also never use this function to pass a pointer to an object that was <b>not</b> created with
	*** the new operator, because it is guaranteed that sooner or later GameGlobal will invoke delete on
	*** this object.
	**/
	void AddToInventory(GlobalObject* object);

	/** \brief Removes an object from the inventory
	*** \param obj_id The identifier value of the object to remove
	*** \note If the object is not in the inventory, the function will do nothing.
	***
	*** This function removes the item regardless of what the GlobalObject#_count member is set to.
	*** If you want to remove only a certain number of instances of the object, use the function
	*** GameGlobal#DecrementObjectCount.
	**/
	void RemoveFromInventory(uint32 obj_id);

	/** \brief Retries a single copy of an object from the inventory
	*** \param obj_id The identifier value of the item to remove
	*** \param all_counts If set to true, all counts of the object will be removed from the inventory (default value == false)
	*** \return A newly instantiated copy of the object, or NULL if the object was not found in the inventory
	***
	*** If all_counts is false, the returned object will have a count of one and the count of the object inside the inventory
	*** will be decremented by one. If all_counts is ture, the returned object will have the same count as was previously in
	*** the inventory, and the object will be removed from the inventory alltogether. Note that the pointer returned will need
	*** to be deleted by the user code, unless the object is re-added to the inventory or equipped on a character.
	**/
	GlobalObject* RetrieveFromInventory(uint32 obj_id, bool all_counts = false);

	/** \brief Increments the number (count) of an object in the inventory
	*** \param item_id The integer identifier of the item that will have its count incremented
	*** \param count The amount to increase the object's count by (default value == 1)
	***
	*** If the item does not exist in the inventory, this function will do nothing. If the count parameter
	*** is set to zero, no change will take place.
	***
	*** \note The callee can not assume that the function call succeeded, but rather has to check this themselves.
	**/
	void IncrementObjectCount(uint32 obj_id, uint32 obj_count = 1);

	/** \brief Decrements the number (count) of an object in the inventory
	*** \param item_id The integer identifier of the item that will have its count decremented
	*** \param count The amount to decrease the object's count by (default value == 1)
	***
	*** If the item does not exist in the inventory, this function will do nothing. If the count parameter
	*** is set to zero, no change will take place. If the count parameter is greater than or equal to the
	*** current count of the object, the object will be completely removed from the inventory.
	***
	*** \note The callee can not assume that the function call succeeded, but rather has to check this themselves.
	**/
	void DecrementObjectCount(uint32 obj_id, uint32 obj_count = 1);

	/** \brief Checks whether or a given object is currently stored in the inventory
	*** \param id The id of the object (item, weapon, armor, etc.) to check for
	*** \return True if the object was found in the inventor, or false if it was not found
	**/
	bool IsObjectInInventory(uint32 id)
		{ if (_inventory.find(id) != _inventory.end()) return true; else return false; }
	//@}

	//! \name Event Group Methods
	//@{
	/** \brief Queries whether or not an event group of a given name exists
	*** \param group_name The name of the event group to check for
	*** \return True if the event group name was found, false if it was not
	**/
	bool DoesEventGroupExist(const std::string& group_name) const
		{ if (_event_groups.find(group_name) != _event_groups.end()) return true; else return false; }

	/** \brief Determines if an event of a given name exists within a given group
	*** \param group_name The name of the event group where the event to check is contained
	*** \param event_name The name of the event to check for
	*** \return True if the event was found, or false if the event name or group name was not found
	**/
	bool DoesEventExist(const std::string& group_name, const std::string& event_name) const;

	/** \brief Adds a new event group for the class to manage
	*** \param group_name The name of the new event group to add
	*** \note If an event group  by the given name already exists, the function will abort
	*** and not add the new event group. Otherwise, this class will automatically construct
	*** a new event group of the given name and place it in its map of event groups.
	**/
	void AddNewEventGroup(const std::string& group_name);

	/** \brief Returns a pointer to an event group of the specified name
	*** \param group_name The name of the event group to retreive
	*** \return A pointer to the GlobalEventGroup that represents the event group, or NULL if no event group
	*** of the specifed name was found
	***
	*** You can use this method to invoke the public methods of the GlobalEventGroup class. For example, if
	*** we wanted to add a new event "cave_collapse" with a value of 1 to the group event "cave_map", we
	*** would do the following: GlobalManager->GetEventGroup("cave_map")->AddNewEvent("cave_collapse", 1);
	*** Be careful, however, because since this function returns NULL if the event group was not found, the
	*** example code above would produce a segmentation fault if no event group by the name "cave_map" existed.
	**/
	GlobalEventGroup* GetEventGroup(const std::string& group_name) const;

	/** \brief Returns the value of an event inside of a specified group
	*** \param group_name The name of the event group where the event is contained
	*** \param event_name The name of the event whose value should be retrieved
	*** \return The value of the requested event, or GLOBAL_BAD_EVENT if the event was not found
	**/
	int32 GetEventValue(const std::string& group_name, const std::string& event_name) const;

	//! \brief Returns the number of event groups stored in the class
	uint32 GetNumberEventGroups() const
		{ return _event_groups.size(); }

	/** \brief Returns the number of events for a specified group name
	*** \param group_name The name of the event group to retrieve the number of events for
	*** \return The number of events in the group, or zero if no such group name existed
	**/
	uint32 GetNumberEvents(const std::string& group_name) const;
	//@}

	//! \note The overflow condition is not checked here: we just assume it will never occur
	void AddDrunes(uint32 amount)
		{ _drunes += amount; }

	//! \note The amount is only subtracted if the current funds is equal to or exceeds the amount to subtract
	void SubtractDrunes(uint32 amount)
		{ if (_drunes >= amount) _drunes -= amount; }

	/** \brief Calculates the average experience level of members in the active party
	*** \return The average (integer) experience level of all members in the active party
	*** This is used for determining the level of growth for enemies in battle.
	**/
	uint32 AverageActivePartyExperienceLevel() const
		{ return static_cast<uint32>(_active_party.AverageExperienceLevel()); }

	/** \brief Sets the name and graphic for the current location
	*** \param location_name The ustring that contains the name of the current map
	*** \param location_graphic_filename The filename of the graphic image that represents this location
	**/
	void SetLocation(const hoa_utils::ustring& location_name, const std::string& location_graphic_filename);

	/** \brief Set the location
	*** \param this is really only used when starting a new game, as we don't know the what the location graphic is yet
	*** the location graphic filename is loaded during map loading.
	**/
	void SetLocation(const hoa_utils::ustring& location_name)
		{ _location_name = location_name; }

	/** \brief Sets the location
	*** \param location_name The string that contains the name of the current map
	**/
	void SetLocation(const std::string& location_name)
		{ _location_name = hoa_utils::MakeUnicodeString(location_name); }

	//! \brief Executes function NewGame() from global script
	void NewGame()
		{ ScriptCallFunction<void>(_global_script.GetLuaState(), "NewGame"); }

	/** \brief Saves all global data to a saved game file
	*** \param filename The filename of the saved game file where to write the data to
	*** \return True if the game was successfully saved, false if it was not
	**/
	bool SaveGame(const std::string& filename);

	/** \brief Loads all global data from a saved game file
	*** \param filename The filename of the saved game file where to read the data from
	*** \return True if the game was successfully loaded, false if it was not
	**/
	bool LoadGame(const std::string& filename);

	//! \name Class Member Access Functions
	//@{
	void SetDrunes(uint32 amount)
		{ _drunes = amount; }

	void SetBattleSetting(GLOBAL_BATTLE_SETTING new_setting)
		{ _battle_setting = new_setting; }

	uint32 GetDrunes() const
		{ return _drunes; }

	hoa_utils::ustring& GetLocationName()
		{ return _location_name; }

	hoa_video::StillImage& GetLocationGraphic()
		{ return _location_graphic; }

	std::vector<GlobalCharacter*>* GetCharacterOrder()
		{ return &_character_order; }

	GLOBAL_BATTLE_SETTING GetBattleSetting() const
		{ return _battle_setting; }

	GlobalParty* GetActiveParty()
		{ return &_active_party; }

	std::map<uint32, GlobalObject*>* GetInventory()
		{ return &_inventory; }

	std::vector<GlobalItem*>* GetInventoryItems()
		{ return &_inventory_items; }

	std::vector<GlobalWeapon*>* GetInventoryWeapons()
		{ return &_inventory_weapons; }

	std::vector<GlobalArmor*>* GetInventoryHeadArmor()
		{ return &_inventory_head_armor; }

	std::vector<GlobalArmor*>* GetInventoryTorsoArmor()
		{ return &_inventory_torso_armor; }

	std::vector<GlobalArmor*>* GetInventoryArmArmor()
		{ return &_inventory_arm_armor; }

	std::vector<GlobalArmor*>* GetInventoryLegArmor()
		{ return &_inventory_leg_armor; }

	std::vector<GlobalShard*>* GetInventoryShards()
		{ return &_inventory_shards; }

	std::vector<GlobalKeyItem*>* GetInventoryKeyItems()
		{ return &_inventory_key_items; }

	hoa_script::ReadScriptDescriptor& GetItemsScript()
		{ return _items_script; }

	hoa_script::ReadScriptDescriptor& GetWeaponsScript()
		{ return _weapons_script; }

	hoa_script::ReadScriptDescriptor& GetHeadArmorScript()
		{ return _head_armor_script; }

	hoa_script::ReadScriptDescriptor& GetTorsoArmorScript()
		{ return _torso_armor_script; }

	hoa_script::ReadScriptDescriptor& GetArmArmorScript()
		{ return _arm_armor_script; }

	hoa_script::ReadScriptDescriptor& GetLegArmorScript()
		{ return _leg_armor_script; }

	hoa_script::ReadScriptDescriptor& GetAttackSkillsScript()
		{ return _attack_skills_script; }

	hoa_script::ReadScriptDescriptor& GetDefendSkillsScript()
		{ return _defend_skills_script; }

	hoa_script::ReadScriptDescriptor& GetSupportSkillsScript()
		{ return _support_skills_script; }

	hoa_script::ReadScriptDescriptor& GetStatusEffectsScript()
		{ return _status_effects_script; }

	hoa_script::ReadScriptDescriptor* GetBattleEventScript()
		{ return &_battle_events_script; }
	//@}

private:
	GameGlobal();

	//! \brief The amount of financial resources (drunes) that the party currently has
	uint32 _drunes;

	//! \brief The name of the map that the current party is on
	hoa_utils::ustring _location_name;

	//! \brief The graphical image which represents the current location
	hoa_video::StillImage _location_graphic;

	//! \brief Retains the play type setting for battle that the user requested (e.g. wait mode, active mode, etc).
	GLOBAL_BATTLE_SETTING _battle_setting;

	/** \brief A map containing all characters that the player has discovered
	*** This map contains all characters that the player has met with, regardless of whether or not they are in the active party.
	*** The map key is the character's unique ID number.
	**/
	std::map<uint32, GlobalCharacter*> _characters;

	/** \brief A vector whose purpose is to maintain the order of characters
	*** The first four characters in this vector are in the active party; the rest are in reserve.
	**/
	std::vector<GlobalCharacter*> _character_order;

	/** \brief The active party of characters
	*** The active party contains the group of characters that will fight when a battle begins.
	*** This party can be up to four characters, and should always contain at least one character.
	**/
	GlobalParty _active_party;

	/** \brief Retains a list of all of the objects currently stored in the player's inventory
	*** This map is used to quickly check if an item is in the inventory or not. The key to the map is the object's
	*** identification number. When an object is added to the inventory, if it already exists then the object counter
	*** is simply increased instead of adding an entire new class object. When the object count becomes zero, the object
	*** is removed from the inventory. Duplicates of all objects are retained in the various inventory containers below.
	**/
	std::map<uint32, GlobalObject*> _inventory;

	/** \brief Inventory containers
	*** These vectors contain the inventory of the entire party. The vectors are sorted according to the player's personal preferences.
	*** When a new object is added to the inventory, by default it will be placed at the end of the vector.
	**/
	//@{
	std::vector<GlobalItem*>     _inventory_items;
	std::vector<GlobalWeapon*>   _inventory_weapons;
	std::vector<GlobalArmor*>    _inventory_head_armor;
	std::vector<GlobalArmor*>    _inventory_torso_armor;
	std::vector<GlobalArmor*>    _inventory_arm_armor;
	std::vector<GlobalArmor*>    _inventory_leg_armor;
	std::vector<GlobalShard*>    _inventory_shards;
	std::vector<GlobalKeyItem*>  _inventory_key_items;
	//@}

	//! \name Global data and function script files
	//@{
	//! \brief Contains character ID definitions and a number of useful functions
	hoa_script::ReadScriptDescriptor _global_script;

	//! \brief Contains data definitions for all items
	hoa_script::ReadScriptDescriptor _items_script;

	//! \brief Contains data definitions for all weapons
	hoa_script::ReadScriptDescriptor _weapons_script;

	//! \brief Contains data definitions for all armor that are equipped on the head
	hoa_script::ReadScriptDescriptor _head_armor_script;

	//! \brief Contains data definitions for all armor that are equipped on the torso
	hoa_script::ReadScriptDescriptor _torso_armor_script;

	//! \brief Contains data definitions for all armor that are equipped on the arms
	hoa_script::ReadScriptDescriptor _arm_armor_script;

	//! \brief Contains data definitions for all armor that are equipped on the legs
	hoa_script::ReadScriptDescriptor _leg_armor_script;

	//! \brief Contains data definitions for all shards
	// hoa_script::ReadScriptDescriptor _shard_script;

	//! \brief Contains data definitions for all key items
	// hoa_script::ReadScriptDescriptor _key_items_script;

	//! \brief Contains data and functional definitions for all attack skills
	hoa_script::ReadScriptDescriptor _attack_skills_script;

	//! \brief Contains data and functional definitions for all defense skills
	hoa_script::ReadScriptDescriptor _defend_skills_script;

	//! \brief Contains data and functional definitions for all support skills
	hoa_script::ReadScriptDescriptor _support_skills_script;

	//! \brief Contains functional definitions for all status effects
	hoa_script::ReadScriptDescriptor _status_effects_script;

	//! \brief Contains data and functional definitions for sprites seen in game maps
	hoa_script::ReadScriptDescriptor _map_sprites_script;

	//! \brief Contains data and functional definitions for scripted events in key game battles
	hoa_script::ReadScriptDescriptor _battle_events_script;
	//@}

	/** \brief The container which stores all of the groups of events that have occured in the game
	*** The name of each GlobalEventGroup object serves as its key in this map data structure.
	**/
	std::map<std::string, GlobalEventGroup*> _event_groups;

	// ----- Private methods

	/** \brief A helper template function that finds and removes an object from the inventory
	*** \param obj_id The ID of the object to remove from the inventory
	*** \param inv The vector container of the appropriate inventory type
	*** \return True if the object was successfully removed, or false if it was not
	**/
	template <class T> bool _RemoveFromInventory(uint32 obj_id, std::vector<T*>& inv);

	/** \brief A helper template function that finds and returns a copy of an object from the inventory
	*** \param obj_id The ID of the object to remove from the inventory
	*** \param inv The vector container of the appropriate inventory type
	*** \param all_counts If false the object's count is decremented by one from the inventory, otherwise all counts are removed completely
	*** \return A pointer to the newly created copy of the object, or NULL if the object could not be found
	**/
	template <class T> T* _RetrieveFromInventory(uint32 obj_id, std::vector<T*>& inv, bool all_counts);

	/** \brief A helper function to GameGlobal::SaveGame() that stores the contents of a type of inventory to the saved game file
	*** \param file A reference to the open and valid file where to write the inventory list
	*** \param name The name under which this set of inventory data should be categorized (ie "items", "weapons", etc)
	*** \param inv A reference to the inventory vector to store
	*** \note The class type T must be a derived class of GlobalObject
	**/
	template <class T> void _SaveInventory(hoa_script::WriteScriptDescriptor& file, std::string name, std::vector<T*>& inv);

	/** \brief A helper function to GameGlobal::SaveGame() that writes character data to the saved game file
	*** \param file A reference to the open and valid file where to write the character data
	*** \param objects A ponter to the character whose data should be saved
	*** \param last Set to true if this is the final character that needs to be saved
	*** This method will need to be called once for each character in the player's party
	**/
	void _SaveCharacter(hoa_script::WriteScriptDescriptor& file, GlobalCharacter* character, bool last);

	/** \brief A helper function to GameGlobal::SaveGame() that writes a group of event data to the saved game file
	*** \param file A reference to the open and valid file where to write the event data
	*** \param event_group A pointer to the group of events to store
	*** This method will need to be called once for each GlobalEventGroup contained by this class.
	**/
	void _SaveEvents(hoa_script::WriteScriptDescriptor& file, GlobalEventGroup* event_group);

	/** \brief A helper function to GameGlobal::LoadGame() that restores the contents of the inventory from a saved game file
	*** \param file A reference to the open and valid file from where to read the inventory list
	*** \param category_name The name of the table in the file that should contain the inventory for a specific category
	**/
	void _LoadInventory(hoa_script::ReadScriptDescriptor& file, std::string category_name);

	/** \brief A helper function to GameGlobal::LoadGame() that loads a saved game character and adds it to the party
	*** \param file A reference to the open and valid file from where to read the character from
	*** \param id The character's integer ID, used to find and restore the character data
	**/
	void _LoadCharacter(hoa_script::ReadScriptDescriptor& file, uint32 id);

	/** \brief A helper function to GameGlobal::LoadGame() that loads a group of game events from a saved game file
	*** \param file A reference to the open and valid file from where to read the event data from
	*** \param group_name The name of the event group to load
	**/
	void _LoadEvents(hoa_script::ReadScriptDescriptor& file, const std::string& group_name);
}; // class GameGlobal : public hoa_utils::Singleton<GameGlobal>

//-----------------------------------------------------------------------------
// Template Function Definitions
//-----------------------------------------------------------------------------

template <class T> bool GameGlobal::_RemoveFromInventory(uint32 obj_id, std::vector<T*>& inv) {
	for (typename std::vector<T*>::iterator i = inv.begin(); i != inv.end(); i++) {
		if ((*i)->GetID() == obj_id) {
			// Delete the object, remove it from the vector container, and remove it from the _inventory map
			delete _inventory[obj_id];
			inv.erase(i);
			_inventory.erase(obj_id);
			return true;
		}
	}

	return false;
} // template <class T> bool GameGlobal::_RemoveFromInventory(uint32 obj_id, std::vector<T*>& inv)



template <class T> T* GameGlobal::_RetrieveFromInventory(uint32 obj_id, std::vector<T*>& inv, bool all_counts) {
	for (typename std::vector<T*>::iterator i = inv.begin(); i != inv.end(); i++) {
		if ((*i)->GetID() == obj_id) {
			T* return_object;
			if (all_counts == true || _inventory[obj_id]->GetCount() == 1) {
				return_object = *i;
				_inventory.erase(obj_id);
				inv.erase(i);
			}
			else {
				return_object = new T(**i);
				return_object->SetCount(1);
				_inventory[obj_id]->DecrementCount();
			}
			return return_object;
		}
	}

	return NULL;
} // template <class T> T* GameGlobal::_RetrieveFromInventory(uint32 obj_id, std::vector<T*>& inv, bool all_counts)



template <class T> void GameGlobal::_SaveInventory(hoa_script::WriteScriptDescriptor& file, std::string name, std::vector<T*>& inv) {
	if (file.IsFileOpen() == false) {
		IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed because the argument file was not open" << std::endl;
		return;
	}

	file.InsertNewLine();
	file.WriteLine(name + " = {");
	for (uint32 i = 0; i < inv.size(); i++) {
		if (i == 0)
			file.WriteLine("\t", false);
		else
			file.WriteLine(", ", false);
		file.WriteLine("[" + hoa_utils::NumberToString(inv[i]->GetID()) + "] = "
			+ hoa_utils::NumberToString(inv[i]->GetCount()), false);
	}
	file.InsertNewLine();
	file.WriteLine("}");
} // template <class T> void GameGlobal::_SaveInventory(hoa_script::WriteScriptDescriptor& file, std::string name, std::vector<T*>& inv)

} // namespace hoa_global

#endif // __GLOBAL_HEADER__
