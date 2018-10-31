////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

#include "script/script_read.h"
#include "script/script_write.h"

#include "media/global_media.h"
#include "media/battle_media.h"

#include "skill_graph/skill_graph.h"
#include "actors/global_character_handler.h"
#include "actors/global_actor.h"
#include "actors/global_party.h"

#include "objects/global_inventory_handler.h"
#include "objects/global_armor.h"
#include "objects/global_weapon.h"

#include "global_skills.h"

#include "global_event_group.h"
#include "quest_log.h"
#include "shop_data.h"
#include "worldmap_location.h"

#include "modes/map/map_utils.h"
#include "modes/map/map_location.h"

//! \brief All calls to global code are wrapped inside this namespace.
namespace vt_global
{

class GameGlobal;
class GlobalSpirit;

//! \brief The singleton pointer responsible for the management of global game data.
extern GameGlobal* GlobalManager;

//! \brief Determines whether the code in the vt_global namespace should print debug statements or not.
extern bool GLOBAL_DEBUG;

/** ****************************************************************************
*** \brief Retains all the state information about the active game
***
*** This class is a resource manager for the current state of the game that is
*** being played.
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

    //! \brief Tells whether an enemy id is existing in the enemy data.
    bool DoesEnemyExist(uint32_t enemy_id);
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
    *** \return A pointer to the GlobalEventGroup that represents the event group, or nullptr if no event group
    *** of the specified name was found
    ***
    *** You can use this method to invoke the public methods of the GlobalEventGroup class. For example, if
    *** we wanted to add a new event "cave_collapse" with a value of 1 to the group event "cave_map", we
    *** would do the following: GlobalManager->GetEventGroup("cave_map")->AddNewEvent("cave_collapse", 1);
    *** Be careful, however, because since this function returns nullptr if the event group was not found, the
    *** example code above would produce a segmentation fault if no event group by the name "cave_map" existed.
    **/
    GlobalEventGroup *GetEventGroup(const std::string &group_name) const;

    /** \brief Returns the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The value of the requested event, or 0 if the event was not found
    **/
    int32_t GetEventValue(const std::string &group_name, const std::string &event_name) const;

    /** \brief Set the value of an event inside of a specified group
    *** \param group_name The name of the event group where the event is contained
    *** \param event_name The name of the event whose value should be retrieved
    *** \return The event value.
    *** \note Events and event groups will be created when necessary.
    **/
    void SetEventValue(const std::string &group_name, const std::string &event_name, int32_t event_value);

    //! \brief Returns the number of event groups stored in the class
    uint32_t GetNumberEventGroups() const {
        return _event_groups.size();
    }

    /** \brief Returns the number of events for a specified group name
    *** \param group_name The name of the event group to retrieve the number of events for
    *** \return The number of events in the group, or zero if no such group name existed
    **/
    uint32_t GetNumberEvents(const std::string &group_name) const;
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
    uint32_t GetNumberQuestLogEntries() const
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
    void AddDrunes(uint32_t amount) {
        _drunes += amount;
    }

    //! \note The amount is only subtracted if the current funds is equal to or exceeds the amount to subtract
    void SubtractDrunes(uint32_t amount) {
        if(_drunes >= amount) _drunes -= amount;
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
    void SetMapDataFilename(const std::string& map_data_filename) {
        _map_data_filename = map_data_filename;
    }

    /** \brief Sets the active Map script filename (for game saves)
    *** \param location_name The string that contains the name of the current map script file
    **/
    void SetMapScriptFilename(const std::string& map_script_filename) {
        _map_script_filename = map_script_filename;
    }

    /** \brief sets the current viewable world map
    *** empty strings are valid, and will cause the return
    *** of a null pointer on GetWorldMap call.
    *** \note this will also clear the currently viewable locations and the current location id
    **/
    void SetWorldMap(const std::string& world_map_filename)
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
    void SetCurrentLocationId(const std::string& location_id)
    {
        _current_world_location_id = location_id;
    }

    /** \brief adds a viewable location string id to the currently viewable
    *** set. This string IDs are maintained in the data/config/world_location.lua file.
    *** \param the string id to the currently viewable location
    **/
    void ShowWorldLocation(const std::string& location_id)
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
    void HideWorldLocation(const std::string &location_id)
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
    const std::vector<std::string>& GetViewableLocationIds() const
    {
        return _viewable_world_locations;
    }

    /** \brief get a pointer to the associated world location for the id
    *** \param string Reference if for the world map location
    *** \return nullptr if the location does not exist. otherwise, return a const pointer
    *** to the location
    **/
    WorldMapLocation* GetWorldLocation(const std::string &id)
    {
        std::map<std::string, WorldMapLocation>::iterator itr = _world_map_locations.find(id);
        return itr == _world_map_locations.end() ? nullptr : &(itr->second);
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

    //! \brief Set the new "home" map data
    void SetHomeMap(const std::string& map_data_filename,
                    const std::string& map_script_filename,
                    uint32_t x_pos, uint32_t y_pos) {
        _home_map = vt_map::MapLocation(map_data_filename,
                                        map_script_filename,
                                        x_pos, y_pos);
    }

    //! \brief Get the current "home" map data
    const vt_map::MapLocation& GetHomeMap() const {
        return _home_map;
    }

    //! \brief Clear Home map data, sometimes used by the game.
    void ClearHomeMap() {
        _home_map.Clear();
    }

    //! \brief Executes function NewGame() from global script
    //! \returns whether it succeeded.
    bool NewGame();

    /** \brief Saves all global data to a saved game file
    *** \param filename The filename of the saved game file where to write the data to
    *** \param slot_id The game slot id used for the save menu.
    *** \param positions When used in a save point, the save map tile positions are given there.
    *** \return True if the game was successfully saved, false if it was not
    **/
    bool SaveGame(const std::string &filename, uint32_t slot_id, uint32_t x_position = 0, uint32_t y_position = 0);

    //! \brief Attempts an autosave on the current slot, using given map and location.
    bool AutoSave(const std::string& map_data_file, const std::string& map_script_file,
                  uint32_t stamina,
                  uint32_t x_position = 0, uint32_t y_position = 0);

    /** \brief Loads all global data from a saved game file
    *** \param filename The filename of the saved game file where to read the data from
    *** \param slot_id The save slot the file correspond to. Used to set the correct cursor position
    *** when further saving.
    *** \return True if the game was successfully loaded, false if it was not
    **/
    bool LoadGame(const std::string &filename, uint32_t slot_id);

    //! \brief Gets the last load/save position.
    uint32_t GetGameSlotId() const {
        return _game_slot_id;
    }

    //! \name Class Member Access Functions
    //@{
    void SetDrunes(uint32_t amount) {
        _drunes = amount;
    }

    void SetMaxExperienceLevel(uint32_t level) {
        _max_experience_level = level;
    }

    uint32_t GetMaxExperienceLevel() const {
        return _max_experience_level;
    }

    uint32_t GetDrunes() const {
        return _drunes;
    }

    const std::string& GetMapDataFilename() const {
        return _map_data_filename;
    }

    const std::string& GetMapScriptFilename() const {
        return _map_script_filename;
    }

    uint32_t GetSaveLocationX() const {
        return _x_save_map_position;
    }

    uint32_t GetSaveLocationY() const {
        return _y_save_map_position;
    }

    uint32_t GetSaveStamina() const {
        return _save_stamina;
    }

    void SetSaveStamina(uint32_t stamina) {
        _save_stamina = stamina;
    }

    /** \brief Unset the save data once retreived at load time.
    *** It should be done in the map code once this data has been restored.
    **/
    void UnsetSaveData() {
        _x_save_map_position = 0;
        _y_save_map_position = 0;
        _save_stamina = 0;
    }

    vt_video::StillImage& GetMapImage() {
        return _map_image;
    }

    const vt_utils::ustring& GetMapHudName() const {
        return _map_hud_name;
    }

    //! \brief gets the current world map image
    //! \return a pointer to the currently viewable World Map Image.
    //! \note returns nullptr if the filename has been set to ""
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

    //! \brief Gives the shop data corresponding to the current shop id.
    // Used to sync a given shop or save games
    const ShopData& GetShopData(const std::string& shop_id) {
        if (_shop_data.find(shop_id) == _shop_data.end())
            return _shop_data[std::string()]; // Return default empty shop data
        return _shop_data.at(shop_id);
    }

    bool HasShopData(const std::string& shop_id) const {
        return (_shop_data.find(shop_id) != _shop_data.end());
    }

    //! \brief Sets the current shop data to global manager.
    void SetShopData(const std::string& shop_id, const ShopData& shop_data);

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
    bool DoesEmoteExist(const std::string& emote_id) {
        return (_emotes.count(emote_id));
    }

    //! \brief Get a pointer reference to the given emote animation. Don't delete it!
    vt_video::AnimatedImage* GetEmoteAnimation(const std::string& emote_id) {
        if(_emotes.find(emote_id) != _emotes.end()) return &_emotes.at(emote_id);
        else return 0;
    }

    //! \brief Get a reference to the skill graph handler
    CharacterHandler& GetCharacterHandler() {
        return _character_handler;
    }

    //! \brief Get a reference to inventory handler
    InventoryHandler& GetInventoryHandler() {
        return _inventory_handler;
    }

    //! \brief Get the reference to the skill graph handler
    SkillGraph& GetSkillGraph() {
        return _skill_graph;
    }

    //! \brief Gives access to global media files.
    //! Note: The reference is passed non const to be able to give modifiable references
    //! and pointers.
    GlobalMedia& Media() {
        return _global_media;
    }

    //! \brief Gives access to global battle media files.
    //! Note: The reference is passed non const to be able to give modifiable references
    //! and pointers.
    BattleMedia& GetBattleMedia() {
        return _battle_media;
    }

private:
    GameGlobal();

    //! \brief The slot id the game was loaded from/saved to, or 0 if none.
    uint32_t _game_slot_id;

    //! \brief The amount of financial resources (drunes) that the party currently has
    uint32_t _drunes;

    /** \brief Set the max level that can be reached by a character
    *** This equals 100 by default, @see Set/GetMaxExperienceLevel()
    **/
    uint32_t _max_experience_level;

    //! \brief The map data and script filename the current party is on.
    std::string _map_data_filename;
    std::string _map_script_filename;

    //! \brief last save point map tile location.
    uint32_t _x_save_map_position;
    uint32_t _y_save_map_position;

    //! \brief last save party stamina value.
    uint32_t _save_stamina;

    //! \brief The graphical image which represents the current location
    vt_video::StillImage _map_image;

    //! \brief The current graphical world map. If the filename is empty,
    //! then we are "hiding" the map
    vt_video::StillImage* _world_map_image;

    //! \brief Contains the previous "home" map location data.
    vt_map::MapLocation _home_map;

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

    //! \name Global data and function script files
    //@{
    //! \brief Contains character ID definitions and a number of useful functions
    vt_script::ReadScriptDescriptor _global_script;

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
    uint32_t _quest_log_count;

    //! \brief A map containing all the emote animations
    std::map<std::string, vt_video::AnimatedImage> _emotes;
    //! \brief The map continaing the four sprite direction offsets (x and y value).
    std::map<std::string, std::vector<std::pair<float, float> > > _emotes_offsets;

    //! \brief a map of the quest string ids to their info
    std::map<std::string, QuestLogInfo> _quest_log_info;

    //! \brief A map of the curent shop data.
    //! shop_id, corresponding shop data
    std::map<std::string, ShopData> _shop_data;

    CharacterHandler _character_handler;

    InventoryHandler _inventory_handler;

    SkillGraph _skill_graph;

    //! \brief Stores whether the map mode minimap should be shown.
    bool _show_minimap;

    // ----- Global media files
    //! \brief member storing all the common media files.
    GlobalMedia _global_media;

    //! \brief member storing all the common battle media files.
    BattleMedia _battle_media;

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
                      uint32_t quest_log_number,
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
    void _SaveQuests(vt_script::WriteScriptDescriptor& file, const QuestLogEntry* quest_log_entry);

    /** \brief saves the world map information. this is called from SaveGame()
    *** \param file Reference to open and valid file for writting the data
    **/
    void _SaveWorldMap(vt_script::WriteScriptDescriptor& file);

    /** \brief saves the shop data information. this is called from SaveGame()
    *** \param file Reference to open and valid file for writting the data
    **/
    void _SaveShopData(vt_script::WriteScriptDescriptor& file);

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

    /** \brief Load shop data from the save game
    *** \param file Reference to an open file for reading save game data
    **/
    void _LoadShopData(vt_script::ReadScriptDescriptor& file);

    //! (Re)Loads the quest entries into the GlobalManager
    //! \Note this is done in _LoadGlobalScripts().
    bool _LoadQuestsScript(const std::string& quests_script_filename);

    //! Loads every persistent scripts, used at the global initialization time.
    bool _LoadGlobalScripts();

    //! Unloads every persistent scripts by closing their files.
    void _CloseGlobalScripts();
};

} // namespace vt_global

#endif // __GLOBAL_HEADER__
