////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for the global game manager
***
*** This file contains the GameGlobal class, which is used to manage all data
*** that is shared "globally" by the various game modes. For example, it
*** contains the current characters in the party, the party's inventory, etc.
*** The definition of characters, items, and other related global data are
*** implemented in the other global header files (e.g. global_actors.h). All
*** of these global files share the same vt_global namespace.
*** ***************************************************************************/

#ifndef __GLOBAL_HEADER__
#define __GLOBAL_HEADER__

#include "utils/utils_strings.h"

#include "engine/script/script_read.h"
#include "engine/script/script_write.h"

#include "global_actors.h"
#include "global_effects.h"
#include "global_objects.h"
#include "global_skills.h"
#include "global_utils.h"

#include "modes/map/map_utils.h"

//! \brief All calls to global code are wrapped inside this namespace.
namespace vt_global
{

class GameGlobal;

//! \brief The singleton pointer responsible for the management of global game data.
extern GameGlobal *GlobalManager;

//! \brief Determines whether the code in the vt_global namespace should print debug statements or not.
extern bool GLOBAL_DEBUG;

/** ****************************************************************************
*** \brief A container that manages the occurences of several related game events
***
*** Events are nothing more than a string-integer pair. The string
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
class GlobalEventGroup
{
public:
    //! \param group_name The name of the group to create (this can not be changed)
    GlobalEventGroup(const std::string &group_name) :
        _group_name(group_name) {}

    ~GlobalEventGroup() {}

    /** \brief Queries whether or not an event of a given name exists in the group
    *** \param event_name The name of the event to check for
    *** \return True if the event name was found in the group, false if it was not
    **/
    bool DoesEventExist(const std::string &event_name) {
        if(_events.find(event_name) != _events.end()) return true;
        else return false;
    }

    /** \brief Adds a new event to the group
    *** \param event_name The name of the event to add
    *** \param event_value The value of the event to add (default value is zero)
    *** \note If an event by the given name already exists, a warning will be printed and no addition
    *** or modification of any kind will take place
    **/
    void AddNewEvent(const std::string &event_name, int32 event_value = 0);

    /** \brief Retrieves the value of a specific event in the group
    *** \param event_name The name of the event to retrieve
    *** \return The value of the event, or 0 if there is no event corresponding to
    *** the requested event named
    **/
    int32 GetEvent(const std::string &event_name);

    /** \brief Sets the value for an existing event
    *** \param event_name The name of the event whose value should be changed
    *** \param event_value The value to set for the event.
    *** \note If the event by the given name is not found, the event group will be created.
    **/
    void SetEvent(const std::string &event_name, int32 event_value);

    //! \brief Returns the number of events currently stored within the group
    uint32 GetNumberEvents() const {
        return _events.size();
    }

    //! \brief Returns a copy of the name of this group
    std::string GetGroupName() const {
        return _group_name;
    }

    //! \brief Returns an immutable reference to the private _events container
    const std::map<std::string, int32>& GetEvents() const {
        return _events;
    }

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
*** \brief An Entry for the quest log
***
*** Similar to Event Groups, QuestLogEntry is a string to multiple-string
*** relationship. Due to lookup speed being a concern, each quest log entry
*** is its own object, with the namespace of the Quest being the key
***
*** A Quest Entry is defined more formally as a Quest name and a "tuple" of
*** the mother dependency event, the completion event, the displaying log entry
*** name and finally the description string
***
*** There should be no need for this to be created outside of the global
*** manager, as the setting and toggling of quests will be done from the game
*** manager and through scrip entries
*** ***************************************************************************/

struct QuestLogEntry
{
public:
    //! ctor to create the QuestLogEntry. the key, event states and description
    //! cannot be changed
    //! \param quest_id for quest entry in config/quest.lua quest table
    //! \param the quest log counter, indicating the order in which the quest was added
    //! \param set whether or not the log entry is read or not. Defaults to false.
    //! this flag is manipulated by the game internals as opposed to the scripting side
    QuestLogEntry(const std::string &quest_id,
                  uint32 quest_number,
                  bool is_read = false):
        _quest_id(quest_id),
        _quest_log_number(quest_number),
        _is_read(is_read)
    {}

    void SetRead()
    { _is_read = true; }

    bool IsRead() const
    { return _is_read; }

    const std::string& GetQuestId() const
    { return _quest_id; }

    uint32 GetQuestLogNumber() const
    { return _quest_log_number; }

private:
    //! the string id for the title and description of this quest
    const std::string _quest_id;

    //! the quest log number for this quest
    const uint32 _quest_log_number;

    //! flag to indicate whether or not this entry has been read
    bool _is_read;
};

// A simple structure used to store quest log system info.
class QuestLogInfo {

public:
    QuestLogInfo(const vt_utils::ustring& title,
                 const vt_utils::ustring& description,
                 const vt_utils::ustring& completion_description,
                 const std::string& completion_event_group,
                 const std::string& completion_event_name,
                 const vt_utils::ustring& location_name,
                 const std::string& location_banner_filename,
                 const vt_utils::ustring& location_subname,
                 const std::string& location_subimage_filename);

    QuestLogInfo()
    {}

    void SetNotCompletableIf(const std::string& not_completable_event_group,
                             const std::string& not_completable_event_name) {
        _not_completable_event_group = not_completable_event_group;
        _not_completable_event_name = not_completable_event_name;
    }

    // User info about the quest log
    vt_utils::ustring _title;
    vt_utils::ustring _description;
    // Completion description gets added to the quest description when the quest is considered completed
    vt_utils::ustring _completion_description;

    // Internal quest info used to know whether the quest is complete.
    std::string _completion_event_group;
    std::string _completion_event_name;
    // Internal quest info used to know whether the quest is not comppletable anymore.
    std::string _not_completable_event_group;
    std::string _not_completable_event_name;

    // location information
    vt_video::StillImage _location_image;
    vt_video::StillImage _location_subimage;
    vt_utils::ustring _location_name;
    vt_utils::ustring _location_subname;
};

/** *****************************************************************************
*** \brief Struct for world map locations
*** the parameters are all immutable and loaded at creation time
*** there should be no reason for these to be created outside the global manager
*** the key is the unique location id set in the script as a string
*** there is no need for accesor functions because this is just a storage struct
*** *****************************************************************************/
struct WorldMapLocation
{
public:
    WorldMapLocation():
        _x(0.0f),
        _y(0.0f)
    {}

    WorldMapLocation(float x, float y, const std::string& location_name,
                     const std::string& image_path, const std::string& world_map_location_id);

    WorldMapLocation(const WorldMapLocation &other):
        _x(other._x),
        _y(other._y),
        _location_name(other._location_name),
        _world_map_location_id(other._world_map_location_id),
        _image(other._image)
    {}

    WorldMapLocation &operator=(const WorldMapLocation &other)
    {
        if(this == &other)
            return *this;
        _x = other._x;
        _y = other._y;
        _location_name = other._location_name;
        _world_map_location_id = other._world_map_location_id;
        _image = other._image;
        return *this;
    }

    ~WorldMapLocation() {
        _image.Clear();
    }

    float _x;
    float _y;
    std::string _location_name;
    std::string _world_map_location_id;
    vt_video::StillImage _image;
};

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
class GameGlobal : public vt_utils::Singleton<GameGlobal>
{
    friend class vt_utils::Singleton<GameGlobal>;

public:
    ~GameGlobal();

    bool SingletonInitialize();

    //! Reloads the persistent scripts. Used when changing the language for instance.
    bool ReloadGlobalScripts()
    { _CloseGlobalScripts(); return _LoadGlobalScripts(); }

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
    void AddCharacter(GlobalCharacter *ch);

    /** \brief Removes a character from the party.
    *** \param id The ID number of the character to remove from the party.
    *** \param erase Tells whether the character should be completely remove
    	or just from the active party.
    **/
    void RemoveCharacter(uint32 id, bool erase = false);

    /** \brief Returns a pointer to a character currently in the party.
    *** \param id The ID number of the character to retrieve.
    *** \return A pointer to the character, or NULL if the character was not found.
    ***/
    GlobalCharacter *GetCharacter(uint32 id);

    /** \brief Swaps the location of two character in the party by their indeces
    *** \param first_index The index of the first character to swap
    *** \param second_index The index of the second character to swap
    **/
    void SwapCharactersByIndex(uint32 first_index, uint32 second_index);

    /** \brief Checks whether or not a character is in the party
    *** \param id The id of the character to check for
    *** \return True if the character was found to be in the party, or false if they were not found.
    **/
    bool IsCharacterInParty(uint32 id) {
        if(_characters.find(id) != _characters.end()) return true;
        else return false;
    }

    //! \brief Tells whether an enemy id is existing in the enemy data.
    bool DoesEnemyExist(uint32 enemy_id);
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
    void AddToInventory(GlobalObject *object);

    /** \brief Removes an object from the inventory
    *** \param obj_id The identifier value of the object to remove
    *** \note If the object is not in the inventory, the function will do nothing.
    ***
    *** This function removes the item regardless of what the GlobalObject#_count member is set to.
    *** If you want to remove only a certain number of instances of the object, use the function
    *** GameGlobal#DecrementObjectCount.
    **/
    void RemoveFromInventory(uint32 obj_id);

    /** \brief Gets a copy of an object from the inventory
    *** \param obj_id The identifier value of the item to obtain
    *** \return A newly instantiated copy of the object, or NULL if the object was not found in the inventory
    **/
    GlobalObject* GetGlobalObject(uint32 obj_id);

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
    bool IsObjectInInventory(uint32 id) {
        return (_inventory.find(id) != _inventory.end());
    }

    /** \brief Gives how many of a given item is in the inventory
    *** \param id The id of the object (item, weapon, armor, etc.) to check for
    *** \return The number of the object found in the inventory
    **/
    uint32 HowManyObjectsInInventory(uint32 id) {
        return (_inventory.find(id) != _inventory.end()) ? _inventory.at(id)->GetCount() : 0;
    }
    //@}

    //! \name Event Group Methods
    //@{
    /** \brief Queries whether or not an event group of a given name exists
    *** \param group_name The name of the event group to check for
    *** \return True if the event group name was found, false if it was not
    **/
    bool DoesEventGroupExist(const std::string &group_name) const {
        return (_event_groups.find(group_name) != _event_groups.end());
    }

    /** \brief Determines if an event of a given name exists within a given group
    *** \param group_name The name of the event group where the event to check is contained
    *** \param event_name The name of the event to check for
    *** \return True if the event was found, or false if the event name or group name was not found
    **/
    bool DoesEventExist(const std::string &group_name, const std::string &event_name) const;

    /** \brief Adds a new event group for the class to manage
    *** \param group_name The name of the new event group to add
    *** \note If an event group  by the given name already exists, the function will abort
    *** and not add the new event group. Otherwise, this class will automatically construct
    *** a new event group of the given name and place it in its map of event groups.
    **/
    void AddNewEventGroup(const std::string &group_name);

    /** \brief Returns a pointer to an event group of the specified name
    *** \param group_name The name of the event group to retreive
    *** \return A pointer to the GlobalEventGroup that represents the event group, or NULL if no event group
    *** of the specified name was found
    ***
    *** You can use this method to invoke the public methods of the GlobalEventGroup class. For example, if
    *** we wanted to add a new event "cave_collapse" with a value of 1 to the group event "cave_map", we
    *** would do the following: GlobalManager->GetEventGroup("cave_map")->AddNewEvent("cave_collapse", 1);
    *** Be careful, however, because since this function returns NULL if the event group was not found, the
    *** example code above would produce a segmentation fault if no event group by the name "cave_map" existed.
    **/
    GlobalEventGroup *GetEventGroup(const std::string &group_name) const;

    /** \brief Returns the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The value of the requested event, or 0 if the event was not found
    **/
    int32 GetEventValue(const std::string &group_name, const std::string &event_name) const;

    /** \brief Set the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The event value.
    *** \note Events and event groups will be created when necessary.
    **/
    void SetEventValue(const std::string &group_name, const std::string &event_name, int32 event_value);

    //! \brief Returns the number of event groups stored in the class
    uint32 GetNumberEventGroups() const {
        return _event_groups.size();
    }

    /** \brief Returns the number of events for a specified group name
    *** \param group_name The name of the event group to retrieve the number of events for
    *** \return The number of events in the group, or zero if no such group name existed
    **/
    uint32 GetNumberEvents(const std::string &group_name) const;
    //@}

    //! \name Quest Log Entry methods
    //@{
    //! \brief Tells whether a quest id is completed, based on the internal quest info
    //! and the current game event values.
    //! \param quest_id the string id into quests table for this quest
    bool IsQuestCompleted(const std::string& quest_id)
    {
        std::map<std::string, vt_global::QuestLogInfo>::iterator it = _quest_log_info.find(quest_id);
        if (it == _quest_log_info.end())
            return false;
        const QuestLogInfo& info = it->second;
        if (info._completion_event_group.empty() || info._completion_event_name.empty())
            return true;

        return (GetEventValue(info._completion_event_group, info._completion_event_name) == 1);
    }

    //! \brief Tells whether a quest id is completed, based on the internal quest info
    //! and the current game event values.
    //! \param quest_id the string id into quests table for this quest
    bool IsQuestCompletable(const std::string& quest_id)
    {
        std::map<std::string, vt_global::QuestLogInfo>::iterator it = _quest_log_info.find(quest_id);
        if (it == _quest_log_info.end())
            return true;
        const QuestLogInfo& info = it->second;
        if (info._not_completable_event_group.empty() || info._not_completable_event_name.empty())
            return true;

        return (GetEventValue(info._not_completable_event_group, info._not_completable_event_name) == 0);
    }

    /** \brief adds a new quest log entry into the quest log entries table
    *** \param quest_id the string id into quests table for this quest
    *** \return true if the entry was added. false if the entry already exists
    **/
    bool AddQuestLog(const std::string &quest_id)
    {
        return _AddQuestLog(quest_id, _quest_log_count++);
    }

    /** \brief gets the number of quest log entries
    *** \return number of log entries
    **/
    uint32 GetNumberQuestLogEntries() const
    {
        return _quest_log_entries.size();
    }

    /** \brief get a list of all the currently active quest log entries
    *** \return a vector of valid quest log entries
    **/
    std::vector<QuestLogEntry *> GetActiveQuestIds() const
    {
        std::vector<QuestLogEntry *> keys;
        for(std::map<std::string, QuestLogEntry *>::const_iterator itr = _quest_log_entries.begin();
                itr != _quest_log_entries.end(); ++itr) {
            if (itr->second)
                keys.push_back(itr->second);
        }
        return keys;
    }

    /** \brief gets a pointer to the description for the quest string id,
    *** \param quest_id the quest id
    *** \return a reference to the given quest log info or an empty quest log info.
    **/
    const QuestLogInfo& GetQuestInfo(const std::string &quest_id) const;
    //@}

    //! \note The overflow condition is not checked here: we just assume it will never occur
    void AddDrunes(uint32 amount) {
        _drunes += amount;
    }

    //! \note The amount is only subtracted if the current funds is equal to or exceeds the amount to subtract
    void SubtractDrunes(uint32 amount) {
        if(_drunes >= amount) _drunes -= amount;
    }

    /** \brief Calculates the average experience level of members in the active party
    *** \return The average (integer) experience level of all members in the active party
    *** This is used for determining the level of growth for enemies in battle.
    **/
    uint32 AverageActivePartyExperienceLevel() const {
        return static_cast<uint32>(_active_party.AverageExperienceLevel());
    }

    /** \brief Sets the name and graphic for the current location
    *** \param map_data_filename The string that contains the name of the current map data file.
    *** \param map_script_filename The string that contains the name of the current map script file.
    *** \param map_image_filename The filename of the image that presents this map
    *** \param map_hud_name The UTF16 map name shown at map intro time.
    **/
    void SetMap(const std::string &map_data_filename,
                const std::string &map_script_filename,
                const std::string &map_image_filename,
                const vt_utils::ustring &map_hud_name);

    /** \brief Sets the active Map data filename (for game saves)
    *** \param location_name The string that contains the name of the current map data
    **/
    void SetMapDataFilename(const std::string &map_data_filename) {
        _map_data_filename = map_data_filename;
    }

    /** \brief Sets the active Map script filename (for game saves)
    *** \param location_name The string that contains the name of the current map script file
    **/
    void SetMapScriptFilename(const std::string &map_script_filename) {
        _map_script_filename = map_script_filename;
    }

    /** \brief sets the current viewable world map
    *** empty strings are valid, and will cause the return
    *** of a null pointer on GetWorldMap call.
    *** \note this will also clear the currently viewable locations and the current location id
    **/
    void SetWorldMap(const std::string &world_map_filename)
    {
        if (_world_map_image)
            delete _world_map_image;

        _viewable_world_locations.clear();
        _current_world_location_id.clear();
        _world_map_image = new vt_video::StillImage();
        _world_map_image->Load(world_map_filename);
    }

    /** \brief Sets the current location id
    *** \param the location id of the world location that is defaulted to as "here"
    *** when the world map menu is opened
    **/
    void SetCurrentLocationId(const std::string &location_id)
    {
        _current_world_location_id = location_id;
    }

    /** \brief adds a viewable location string id to the currently viewable
    *** set. This string IDs are maintained in the dat/ folder
    *** \param the string id to the currently viewable location
    **/
    void ShowWorldLocation(const std::string &location_id)
    {
        //defensive check. do not allow blank ids.
        //if you want to remove an id, call HideWorldLocation
        if(location_id.empty())
            return;
        // check to make sure this location isn't already visible
        if(std::find(_viewable_world_locations.begin(),
                     _viewable_world_locations.end(),
                     location_id) == _viewable_world_locations.end())
        {
            _viewable_world_locations.push_back(location_id);
        }

    }

    /** \brief removes a location from the currently viewable list
    *** if the id doesn't exist, we don't do anything
    *** \param the string id to the viewable location we want to hide
    **/
    void HideWorldLocation( const std::string &location_id)
    {
        std::vector<std::string>::iterator rem_iterator = std::find(_viewable_world_locations.begin(),
                                                          _viewable_world_locations.end(),
                                                          location_id);
        if(rem_iterator != _viewable_world_locations.end())
            _viewable_world_locations.erase((rem_iterator));
    }

    /** \brief gets a reference to the current viewable location ids
    *** \return reference to the current viewable location ids
    **/
    const std::vector<std::string> &GetViewableLocationIds() const
    {
        return _viewable_world_locations;
    }

    /** \brief get a pointer to the associated world location for the id
    *** \param string Reference if for the world map location
    *** \return NULL if the location does not exist. otherwise, return a const pointer
    *** to the location
    **/
    WorldMapLocation *GetWorldLocation(const std::string &id)
    {
        std::map<std::string, WorldMapLocation>::iterator itr = _world_map_locations.find(id);
        return itr == _world_map_locations.end() ? NULL : &(itr->second);
    }

    /** \brief Gets a reference to the current world location id
    *** \return Reference to the current id. this value always exists, but could be "" if
    *** the location is not set, or if the world map is cleared
    *** the value could also not currently exist, if HideWorldLocation was called on an
    *** id that was also set as the current location. the calling code should check for this
    **/
    const std::string &GetCurrentLocationId() const
    {
        return _current_world_location_id;
    }

    /** Set up the previous map point the character is coming from.
    *** It is used to make the new map aware about where the character should appear.
    ***
    *** \param previous_location The string telling the location the character is coming from.
    **/
    void SetPreviousLocation(const std::string &previous_location) {
        _previous_location = previous_location;
    }

    const std::string &GetPreviousLocation() const {
        return _previous_location;
    }

    /** Get the previous map hud name shown at intro time.
    *** Used to know whether the new hud name is the same in the map mode.
    **/
    bool ShouldDisplayHudNameOnMapIntro() const {
        return !_same_map_hud_name_as_previous;
    }

    //! Tells whether the map mode minimap should be shown, if any.
    bool ShouldShowMinimap() const {
        return _show_minimap;
    }

    void ShowMinimap(bool show) {
        _show_minimap = show;
    }

    //! \brief Executes function NewGame() from global script
    void NewGame();

    /** \brief Saves all global data to a saved game file
    *** \param filename The filename of the saved game file where to write the data to
    *** \param slot_id The game slot id used for the save menu.
    *** \param positions When used in a save point, the save map tile positions are given there.
    *** \return True if the game was successfully saved, false if it was not
    **/
    bool SaveGame(const std::string &filename, uint32 slot_id, uint32 x_position = 0, uint32 y_position = 0);

    /** \brief Loads all global data from a saved game file
    *** \param filename The filename of the saved game file where to read the data from
    *** \param slot_id The save slot the file correspond to. Used to set the correct cursor position
    *** when further saving.
    *** \return True if the game was successfully loaded, false if it was not
    **/
    bool LoadGame(const std::string &filename, uint32 slot_id);

    uint32 GetGameSlotId() const {
        return _game_slot_id;
    }

    //! \name Class Member Access Functions
    //@{
    void SetDrunes(uint32 amount) {
        _drunes = amount;
    }

    void SetMaxExperienceLevel(uint32 level) {
        _max_experience_level = level;
    }

    uint32 GetMaxExperienceLevel() const {
        return _max_experience_level;
    }

    uint32 GetDrunes() const {
        return _drunes;
    }

    const std::string &GetMapDataFilename() {
        return _map_data_filename;
    }

    const std::string &GetMapScriptFilename() {
        return _map_script_filename;
    }

    uint32 GetSaveLocationX() {
        return _x_save_map_position;
    }

    uint32 GetSaveLocationY() {
        return _y_save_map_position;
    }

    /** \brief Unset the save location once retreived at load time.
    *** It should be done in the map code once the location has been set up.
    **/
    void UnsetSaveLocation() {
        _x_save_map_position = 0;
        _y_save_map_position = 0;
    }

    vt_video::StillImage &GetMapImage() {
        return _map_image;
    }

    const vt_utils::ustring& GetMapHudName() const {
        return _map_hud_name;
    }

    //! \brief gets the current world map image
    //! \return a pointer to the currently viewable World Map Image.
    //! \note returns NULL if the filename has been set to ""
    vt_video::StillImage *GetWorldMapImage() const
    {
        return _world_map_image;
    }

    const std::string &GetWorldMapFilename() const
    {
        if (_world_map_image)
            return _world_map_image->GetFilename();
        else
            return vt_utils::_empty_string;
    }

    std::vector<GlobalCharacter *>* GetOrderedCharacters() {
        return &_ordered_characters;
    }

    // Returns the character party position
    uint32 GetPartyPosition(GlobalCharacter* character) {
        for (uint32 i = 0; i < _ordered_characters.size(); ++i) {
            if (_ordered_characters[i] == character)
                return i;
        }
        // Default case
        return 0;
    }

    GlobalParty *GetActiveParty() {
        return &_active_party;
    }

    std::map<uint32, GlobalObject *>* GetInventory() {
        return &_inventory;
    }

    std::vector<GlobalItem *>* GetInventoryItems() {
        return &_inventory_items;
    }

    std::vector<GlobalWeapon *>* GetInventoryWeapons() {
        return &_inventory_weapons;
    }

    std::vector<GlobalArmor *>* GetInventoryHeadArmor() {
        return &_inventory_head_armor;
    }

    std::vector<GlobalArmor *>* GetInventoryTorsoArmor() {
        return &_inventory_torso_armor;
    }

    std::vector<GlobalArmor *>* GetInventoryArmArmor() {
        return &_inventory_arm_armor;
    }

    std::vector<GlobalArmor *>* GetInventoryLegArmor() {
        return &_inventory_leg_armor;
    }

    std::vector<GlobalSpirit *>* GetInventorySpirits() {
        return &_inventory_spirits;
    }

    vt_script::ReadScriptDescriptor &GetItemsScript() {
        return _items_script;
    }

    vt_script::ReadScriptDescriptor &GetWeaponsScript() {
        return _weapons_script;
    }

    vt_script::ReadScriptDescriptor &GetHeadArmorScript() {
        return _head_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetTorsoArmorScript() {
        return _torso_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetArmArmorScript() {
        return _arm_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetLegArmorScript() {
        return _leg_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetSpiritsScript() {
        return _spirits_script;
    }

    vt_script::ReadScriptDescriptor &GetWeaponSkillsScript() {
        return _weapon_skills_script;
    }

    vt_script::ReadScriptDescriptor &GetMagicSkillsScript() {
        return _magic_skills_script;
    }

    vt_script::ReadScriptDescriptor &GetSpecialSkillsScript() {
        return _special_skills_script;
    }

    vt_script::ReadScriptDescriptor &GetBareHandsSkillsScript() {
        return _bare_hands_skills_script;
    }

    vt_script::ReadScriptDescriptor &GetStatusEffectsScript() {
        return _status_effects_script;
    }

    vt_script::ReadScriptDescriptor &GetCharactersScript() {
        return _characters_script;
    }

    vt_script::ReadScriptDescriptor &GetEnemiesScript() {
        return _enemies_script;
    }

    vt_script::ReadScriptDescriptor &GetMapSpriteScript() {
        return _map_sprites_script;
    }
    //@}

    //! \brief loads the emotes used for character feelings expression in the given lua file.
    void LoadEmotes(const std::string &emotes_filename);

    //! \brief Set up the offsets for the given emote animation and sprite direction.
    void GetEmoteOffset(float &x, float &y, const std::string &emote_id, vt_map::private_map::ANIM_DIRECTIONS dir);

    //! \brief Tells whether an emote id exists and is valid
    bool DoesEmoteExist(const std::string &emote_id) {
        return (_emotes.count(emote_id));
    }

    //! \brief Get a pointer reference to the given emote animation. Don't delete it!
    vt_video::AnimatedImage *GetEmoteAnimation(const std::string &emote_id) {
        if(_emotes.find(emote_id) != _emotes.end()) return &_emotes.at(emote_id);
        else return 0;
    }

    //! \brief Gives access to global media files.
    //! Note: The reference is passed non const to be able to give modifiable references
    //! and pointers.
    GlobalMedia& Media() {
        return _global_media;
    }

private:
    GameGlobal();

    //! \brief The slot id the game was loaded from/saved to, or 0 if none.
    uint32 _game_slot_id;

    //! \brief The amount of financial resources (drunes) that the party currently has
    uint32 _drunes;

    /** \brief Set the max level that can be reached by a character
    *** This equals 100 by default, @see Set/GetMaxExperienceLevel()
    **/
    uint32 _max_experience_level;

    //! \brief The map data and script filename the current party is on.
    std::string _map_data_filename;
    std::string _map_script_filename;

    //! \brief last save point map tile location.
    uint32 _x_save_map_position, _y_save_map_position;

    //! \brief The graphical image which represents the current location
    vt_video::StillImage _map_image;

    //! \brief The current graphical world map. If the filename is empty,
    //! then we are "hiding" the map
    vt_video::StillImage* _world_map_image;

    //! \brief The current viewable location ids on the current world map image
    //! \note this list is cleared when we call SetWorldMap. It is up to the
    //! script writter to maintain the properties of the map by either
    //!  1) call CopyViewableLocationList()
    //!  2) maintain in some other fashion the list

    std::vector<std::string> _viewable_world_locations;

    //! \brief The map location the character is com from. Used to make the new map know where to make the character appear.
    std::string _previous_location;

    /** \brief Stores the previous and current map names appearing on screen at intro time.
    *** This is used to know whether we have to display it, as we won't when it's the same location name than the previous map.
    **/
    vt_utils::ustring _previous_map_hud_name;
    vt_utils::ustring _map_hud_name;
    bool _same_map_hud_name_as_previous;

    /** \brief A map containing all characters that the player has discovered
    *** This map contains all characters that the player has met with, regardless of whether or not they are in the active party.
    *** The map key is the character's unique ID number.
    **/
    std::map<uint32, GlobalCharacter *> _characters;

    /** \brief A vector whose purpose is to maintain the order of characters
    *** The first four characters in this vector are in the active party; the rest are in reserve.
    **/
    std::vector<GlobalCharacter *> _ordered_characters;

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
    std::map<uint32, GlobalObject *> _inventory;

    /** \brief Inventory containers
    *** These vectors contain the inventory of the entire party. The vectors are sorted according to the player's personal preferences.
    *** When a new object is added to the inventory, by default it will be placed at the end of the vector.
    **/
    //@{
    std::vector<GlobalItem *>     _inventory_items;
    std::vector<GlobalWeapon *>   _inventory_weapons;
    std::vector<GlobalArmor *>    _inventory_head_armor;
    std::vector<GlobalArmor *>    _inventory_torso_armor;
    std::vector<GlobalArmor *>    _inventory_arm_armor;
    std::vector<GlobalArmor *>    _inventory_leg_armor;
    std::vector<GlobalSpirit *>   _inventory_spirits;
    //@}

    //! \name Global data and function script files
    //@{
    //! \brief Contains character ID definitions and a number of useful functions
    vt_script::ReadScriptDescriptor _global_script;

    //! \brief Contains data definitions for all items
    vt_script::ReadScriptDescriptor _items_script;

    //! \brief Contains data definitions for all weapons
    vt_script::ReadScriptDescriptor _weapons_script;

    //! \brief Contains data definitions for all armor that are equipped on the head
    vt_script::ReadScriptDescriptor _head_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the torso
    vt_script::ReadScriptDescriptor _torso_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the arms
    vt_script::ReadScriptDescriptor _arm_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the legs
    vt_script::ReadScriptDescriptor _leg_armor_script;

    //! \brief Contains data definitions for all spirits
    vt_script::ReadScriptDescriptor _spirits_script;

    //! \brief Contains data and functional definitions for all weapon skills
    vt_script::ReadScriptDescriptor _weapon_skills_script;

    //! \brief Contains data and functional definitions for all magic skills
    vt_script::ReadScriptDescriptor _magic_skills_script;

    //! \brief Contains data and functional definitions for all special skills
    vt_script::ReadScriptDescriptor _special_skills_script;

    //! \brief Contains data and functional definitions for all bare hands skills
    vt_script::ReadScriptDescriptor _bare_hands_skills_script;

    //! \brief Contains functional definitions for all status effects
    vt_script::ReadScriptDescriptor _status_effects_script;

    //! \brief Contains data and functional definitions for characters
    vt_script::ReadScriptDescriptor _characters_script;

    //! \brief Contains data and functional definitions for enemies
    vt_script::ReadScriptDescriptor _enemies_script;

    //! \brief Contains data and functional definitions for sprites seen in game maps
    vt_script::ReadScriptDescriptor _map_sprites_script;

    //! \brief Contains data and functional definitions for map objects seen in game maps
    vt_script::ReadScriptDescriptor _map_objects_script;

    //! \brief Contains data and functional definitions for map treasures seen in game maps
    vt_script::ReadScriptDescriptor _map_treasures_script;
    //@}

    /** \brief The container which stores all of the groups of events that have occured in the game
    *** The name of each GlobalEventGroup object serves as its key in this map data structure.
    **/
    std::map<std::string, GlobalEventGroup *> _event_groups;

    /** \brief The container which stores the quest log entries in the game. the quest log key
    *** acts as the key for this quest
    *** \note due to a limitation with OptionBoxes, we can only currently only support 255
    *** entries. Please be careful about this limitation
    **/
    std::map<std::string, QuestLogEntry *> _quest_log_entries;

    /** \brief the container which stores all the available world locations in the game.
    *** the world_location_id acts as the key
    **/
    std::map<std::string, WorldMapLocation> _world_map_locations;

    //! \brief the current world map location id that indicates where the player is
    std::string _current_world_location_id;

    /** \brief counter that is updated as quest log entries are added. we use this to
    *** order the quest logs from recent (high number) to older (low number)
    **/
    uint32 _quest_log_count;

    //! \brief A map containing all the emote animations
    std::map<std::string, vt_video::AnimatedImage> _emotes;
    //! \brief The map continaing the four sprite direction offsets (x and y value).
    std::map<std::string, std::vector<std::pair<float, float> > > _emotes_offsets;

    //! \brief a map of the quest string ids to their info
    std::map<std::string, QuestLogInfo> _quest_log_info;

    //! \brief Stores whether the map mode minimap should be shown.
    bool _show_minimap;

    // ----- Global media files
    //! \brief member storing all the common media files.
    GlobalMedia _global_media;

    // ----- Private methods

    /** \brief A helper template function that finds and removes an object from the inventory
    *** \param obj_id The ID of the object to remove from the inventory
    *** \param inv The vector container of the appropriate inventory type
    *** \return True if the object was successfully removed, or false if it was not
    **/
    template <class T> bool _RemoveFromInventory(uint32 obj_id, std::vector<T *>& inv);

    /** \brief A helper template function that finds and returns a copy of an object from the inventory
    *** \param obj_id The ID of the object to obtain from the inventory
    *** \param inv The vector container of the appropriate inventory type
    *** \return A pointer to the newly created copy of the object, or NULL if the object could not be found
    **/
    template <class T> T *_GetFromInventory(uint32 obj_id, std::vector<T *>& inv);

    /** \brief A helper function to GameGlobal::SaveGame() that stores the contents of a type of inventory to the saved game file
    *** \param file A reference to the open and valid file where to write the inventory list
    *** \param name The name under which this set of inventory data should be categorized (ie "items", "weapons", etc)
    *** \param inv A reference to the inventory vector to store
    *** \note The class type T must be a derived class of GlobalObject
    **/
    template <class T> void _SaveInventory(vt_script::WriteScriptDescriptor &file, const std::string &name, std::vector<T *>& inv);

    /** \brief A helper function to GameGlobal::SaveGame() that writes character data to the saved game file
    *** \param file A reference to the open and valid file where to write the character data
    *** \param objects A ponter to the character whose data should be saved
    *** \param last Set to true if this is the final character that needs to be saved
    *** This method will need to be called once for each character in the player's party
    **/
    void _SaveCharacter(vt_script::WriteScriptDescriptor &file, GlobalCharacter *character, bool last);

    /** \brief A helper function to GameGlobal::SaveGame() that writes a group of event data to the saved game file
    *** \param file A reference to the open and valid file where to write the event data
    *** \param event_group A pointer to the group of events to store
    *** This method will need to be called once for each GlobalEventGroup contained by this class.
    **/
    void _SaveEvents(vt_script::WriteScriptDescriptor &file, GlobalEventGroup *event_group);

    /** \brief adds a new quest log entry into the quest log entries table. also updates the quest log number
    *** \param quest_id for the quest
    *** \param the quest entry's log number
    *** \param flag to indicate if this entry is read or not. default is false
    *** \return true if the entry was added. false if the entry already exists
    **/
    bool _AddQuestLog(const std::string &quest_id,
                      uint32 quest_log_number,
                      bool is_read = false)
    {
        if(_quest_log_entries.find(quest_id) != _quest_log_entries.end())
            return false;
        _quest_log_entries[quest_id] = new QuestLogEntry(quest_id,
                                                         quest_log_number,
                                                         is_read);
        return true;
    }

    /** \brief Helper function that saves the Quest Log entries. this is called from SaveGame()
    *** \param file Reference to open and valid file set for writting the data
    *** \param the quest log entry we wish to write
    **/
    void _SaveQuests(vt_script::WriteScriptDescriptor &file, const QuestLogEntry *quest_log_entry);

    /** \brief saves the world map information. this is called from SaveGame()
    *** \param file Reference to open and valid file for writting the data
    **/
    void _SaveWorldMap(vt_script::WriteScriptDescriptor &file);

    /** \brief A helper function to GameGlobal::LoadGame() that restores the contents of the inventory from a saved game file
    *** \param file A reference to the open and valid file from where to read the inventory list
    *** \param category_name The name of the table in the file that should contain the inventory for a specific category
    **/
    void _LoadInventory(vt_script::ReadScriptDescriptor &file, const std::string &category_name);

    /** \brief A helper function to GameGlobal::LoadGame() that loads a saved game character and adds it to the party
    *** \param file A reference to the open and valid file from where to read the character from
    *** \param id The character's integer ID, used to find and restore the character data
    **/
    void _LoadCharacter(vt_script::ReadScriptDescriptor &file, uint32 id);

    /** \brief A helper function to GameGlobal::LoadGame() that loads a group of game events from a saved game file
    *** \param file A reference to the open and valid file from where to read the event data from
    *** \param group_name The name of the event group to load
    **/
    void _LoadEvents(vt_script::ReadScriptDescriptor &file, const std::string &group_name);

    /** \brief Helper function called by LoadGame() that loads each quest into the quest entry table based on the quest_entry_keys
    *** in the save game file
    *** \param file Reference to open and valid file set for reading the data
    *** \param reference to the quest entry key
    **/
    void _LoadQuests(vt_script::ReadScriptDescriptor &file, const std::string &quest_key);

    /** \brief Load world map and viewable information from the save game
    *** \param file Reference to an open file for reading save game data
    **/
    void _LoadWorldMap(vt_script::ReadScriptDescriptor &file);

    /** \brief Helper function called by LoadGlobalScripts() that (re)loads each world location from the script into the world location entry map
    *** \param file Path to the file to world locations script
    *** \return true if successfully loaded
    **/
    bool _LoadWorldLocationsScript(const std::string &world_locations_filename);

    //! (Re)Loads the quest entries into the GlobalManager
    //! \Note this is done in _LoadGlobalScripts().
    bool _LoadQuestsScript(const std::string& quests_script_filename);

    //! Loads every persistent scripts, used at the global initialization time.
    bool _LoadGlobalScripts();

    //! Unloads every persistent scripts by closing their files.
    void _CloseGlobalScripts();
}; // class GameGlobal : public vt_utils::Singleton<GameGlobal>

//-----------------------------------------------------------------------------
// Template Function Definitions
//-----------------------------------------------------------------------------

template <class T> bool GameGlobal::_RemoveFromInventory(uint32 obj_id, std::vector<T *>& inv)
{
    for(typename std::vector<T *>::iterator i = inv.begin(); i != inv.end(); i++) {
        if((*i)->GetID() == obj_id) {
            // Delete the object, remove it from the vector container, and remove it from the _inventory map
            delete _inventory[obj_id];
            inv.erase(i);
            _inventory.erase(obj_id);
            return true;
        }
    }

    return false;
} // template <class T> bool GameGlobal::_RemoveFromInventory(uint32 obj_id, std::vector<T*>& inv)

template <class T> T *GameGlobal::_GetFromInventory(uint32 obj_id, std::vector<T *>& inv)
{
    for(typename std::vector<T*>::iterator it = inv.begin(); it != inv.end(); ++it) {
        if((*it)->GetID() != obj_id)
            continue;

        T *return_object = new T(**it);
        return_object->SetCount(1);
        return return_object;
    }

    return NULL;
}

template <class T> void GameGlobal::_SaveInventory(vt_script::WriteScriptDescriptor &file, const std::string &name, std::vector<T *>& inv)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed because the argument file was not open" << std::endl;
        return;
    }

    file.InsertNewLine();
    file.WriteLine(name + " = {");
    for(uint32 i = 0; i < inv.size(); i++) {
        // Don't save inventory items with 0 count
        if(inv[i]->GetCount() == 0)
            continue;

        if(i == 0)
            file.WriteLine("\t", false);
        else
            file.WriteLine(", ", false);

        // Add a new line every 10 entries for better readability and debugging
        if ((i > 0) && !(i % 10)) {
            file.InsertNewLine();
            file.WriteLine("\t", false);
        }

        file.WriteLine("[" + vt_utils::NumberToString(inv[i]->GetID()) + "] = "
                       + vt_utils::NumberToString(inv[i]->GetCount()), false);
    }
    file.InsertNewLine();
    file.WriteLine("},");
} // template <class T> void GameGlobal::_SaveInventory(vt_script::WriteScriptDescriptor& file, std::string name, std::vector<T*>& inv)

} // namespace vt_global

#endif // __GLOBAL_HEADER__
