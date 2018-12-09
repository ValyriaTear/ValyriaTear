////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

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

#include "events/global_events.h"
#include "quests/quests.h"
#include "worldmap/worldmap_handler.h"
#include "maps/map_data_handler.h"
#include "shop/shop_data_handler.h"
#include "emotes/emote_handler.h"

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

    //! \brief Executes function NewGame() from global script
    //! \returns whether it succeeded.
    bool NewGame();

    /** \brief Loads all global data from a saved game file
    *** \param filename The filename of the saved game file where to read the data from
    *** \param slot_id The save slot the file correspond to. Used to set the correct cursor position
    *** when further saving.
    *** \return True if the game was successfully loaded, false if it was not
    **/
    bool LoadGame(const std::string &filename, uint32_t slot_id);

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

    //! \brief Gets the last load/save position.
    uint32_t GetGameSlotId() const {
        return _game_slot_id;
    }

    //! \note The overflow condition is not checked here: we just assume it will never occur
    void AddDrunes(uint32_t amount) {
        _drunes += amount;
    }

    //! \note The amount is only subtracted if the current funds is equal to or exceeds the amount to subtract
    void SubtractDrunes(uint32_t amount) {
        if(_drunes >= amount) _drunes -= amount;
    }

    void SetDrunes(uint32_t amount) {
        _drunes = amount;
    }

    uint32_t GetDrunes() const {
        return _drunes;
    }

    void SetMaxExperienceLevel(uint32_t level) {
        _max_experience_level = level;
    }

    uint32_t GetMaxExperienceLevel() const {
        return _max_experience_level;
    }

    //! \brief Tells whether an enemy id is existing in the enemy data.
    bool DoesEnemyExist(uint32_t enemy_id);

    vt_script::ReadScriptDescriptor& GetWeaponSkillsScript() {
        return _weapon_skills_script;
    }

    vt_script::ReadScriptDescriptor& GetMagicSkillsScript() {
        return _magic_skills_script;
    }

    vt_script::ReadScriptDescriptor& GetSpecialSkillsScript() {
        return _special_skills_script;
    }

    vt_script::ReadScriptDescriptor& GetBareHandsSkillsScript() {
        return _bare_hands_skills_script;
    }

    vt_script::ReadScriptDescriptor& GetStatusEffectsScript() {
        return _status_effects_script;
    }

    vt_script::ReadScriptDescriptor& GetCharactersScript() {
        return _characters_script;
    }

    vt_script::ReadScriptDescriptor& GetEnemiesScript() {
        return _enemies_script;
    }

    vt_script::ReadScriptDescriptor& GetMapSpriteScript() {
        return _map_sprites_script;
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

    GameEvents& GetGameEvents() {
        return _game_events;
    }

    GameQuests& GetGameQuests() {
        return _game_quests;
    }

    MapDataHandler& GetMapData() {
        return _map_data_handler;
    }

    WorldMapHandler& GetWorldMapData() {
        return _worldmap_handler;
    }

    ShopDataHandler& GetShopDataHandler() {
        return _shop_data_handler;
    }

    EmoteHandler& GetEmoteHandler() {
        return _emote_handler;
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

    //! \brief The container which stores all of the groups of events that have occured in the game
    GameEvents _game_events;

    CharacterHandler _character_handler;

    InventoryHandler _inventory_handler;

    SkillGraph _skill_graph;

    GameQuests _game_quests;

    MapDataHandler _map_data_handler;

    WorldMapHandler _worldmap_handler;

    ShopDataHandler _shop_data_handler;

    EmoteHandler _emote_handler;

    //! \brief member storing all the common media files.
    GlobalMedia _global_media;

    //! \brief member storing all the common battle media files.
    BattleMedia _battle_media;

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

    //! \brief Loads every persistent scripts, used at the global initialization time.
    bool _LoadGlobalScripts();

    //! \brief Unloads every persistent scripts by closing their files.
    void _CloseGlobalScripts();
};

} // namespace vt_global

#endif // __GLOBAL_HEADER__
