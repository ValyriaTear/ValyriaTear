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
*** \file    global.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for the global game manager
*** ***************************************************************************/

#include "global.h"

#include "actors/global_character.h"

#include "objects/global_weapon.h"
#include "objects/global_armor.h"
#include "objects/global_spirit.h"

#include "engine/system.h"
#include "modes/map/map_mode.h"

#include "common/app_settings.h"

using namespace vt_utils;
using namespace vt_common;

using namespace vt_video;
using namespace vt_script;
using namespace vt_system;

namespace vt_global
{

GameGlobal* GlobalManager = nullptr;
bool GLOBAL_DEBUG = false;

GameGlobal::GameGlobal() :
    _game_slot_id(std::numeric_limits<uint32_t>::max()),
    _drunes(0),
    _max_experience_level(100),
    _x_save_map_position(0),
    _y_save_map_position(0),
    _save_stamina(0),
    _world_map_image(nullptr),
    _same_map_hud_name_as_previous(false),
    _quest_log_count(0),
    _show_minimap(true)
{
    IF_PRINT_DEBUG(GLOBAL_DEBUG) << "GameGlobal constructor invoked" << std::endl;
}

GameGlobal::~GameGlobal()
{
    IF_PRINT_DEBUG(GLOBAL_DEBUG) << "GameGlobal destructor invoked" << std::endl;

    ClearAllData();

    _CloseGlobalScripts();
}

bool GameGlobal::SingletonInitialize()
{
    // Init the media files.
    _global_media.Initialize();
    _battle_media.Initialize();

    return _LoadGlobalScripts();
}

void GameGlobal::_CloseGlobalScripts() {
    // Close all persistent script files
    _global_script.CloseFile();

    _inventory_handler.CloseScripts();

    _weapon_skills_script.CloseTable();
    _weapon_skills_script.CloseFile();

    _magic_skills_script.CloseTable();
    _magic_skills_script.CloseFile();

    _special_skills_script.CloseTable();
    _special_skills_script.CloseFile();

    _bare_hands_skills_script.CloseTable();
    _bare_hands_skills_script.CloseFile();

    _status_effects_script.CloseTable();
    _status_effects_script.CloseFile();

    _characters_script.CloseTable();
    _characters_script.CloseFile();

    _enemies_script.CloseTable();
    _enemies_script.CloseFile();

    _map_sprites_script.CloseFile();
    _map_objects_script.CloseFile();
    _map_treasures_script.CloseFile();
}

bool GameGlobal::_LoadGlobalScripts()
{
    // Open up the persistent script files
    if(!_global_script.OpenFile("data/global.lua"))
        return false;

    if (!_inventory_handler.LoadScripts())
        return false;

    if(!_weapon_skills_script.OpenFile("data/skills/weapon.lua") || !_weapon_skills_script.OpenTable("skills"))
        return false;

    if(!_magic_skills_script.OpenFile("data/skills/magic.lua") || !_magic_skills_script.OpenTable("skills"))
       return false;

    if(!_special_skills_script.OpenFile("data/skills/special.lua") || !_special_skills_script.OpenTable("skills"))
        return false;

    if(!_bare_hands_skills_script.OpenFile("data/skills/barehands.lua") || !_bare_hands_skills_script.OpenTable("skills"))
        return false;

    if(!_status_effects_script.OpenFile("data/entities/status_effects/status_effects.lua") || !_status_effects_script.OpenTable("status_effects"))
        return false;

    if(!_characters_script.OpenFile("data/entities/characters.lua") || !_characters_script.OpenTable("characters"))
        return false;

    if(!_enemies_script.OpenFile("data/entities/enemies.lua") || !_enemies_script.OpenTable("enemies"))
        return false;

    if(!_map_sprites_script.OpenFile("data/entities/map_sprites.lua") || !_map_sprites_script.OpenTable("sprites"))
        return false;

    if(!_map_objects_script.OpenFile("data/entities/map_objects.lua"))
        return false;

    if(!_map_treasures_script.OpenFile("data/entities/map_treasures.lua"))
        return false;

    // Reload the Quests script
    if(!_LoadQuestsScript("data/config/quests.lua"))
        return false;

    if(!_LoadWorldLocationsScript("data/config/world_locations.lua"))
        return false;

    if (!_skill_graph.Initialize("data/config/skill_graph.lua"))
        return false;

    return true;
}

void GameGlobal::ClearAllData()
{
    _inventory_handler.ClearAllData();

    _character_handler.ClearAllData();

    // Delete all event groups
    for(std::map<std::string, GlobalEventGroup *>::iterator it = _event_groups.begin(); it != _event_groups.end(); ++it) {
        delete(it->second);
    }
    _event_groups.clear();

    // Clear the quest log
    for(std::map<std::string, QuestLogEntry *>::iterator itr = _quest_log_entries.begin(); itr != _quest_log_entries.end(); ++itr)
        delete itr->second;
    _quest_log_entries.clear();

    // Clear the save temporary data
    UnsetSaveData();

    // Clear out the map previous location
    _previous_location.clear();
    _map_data_filename.clear();
    _map_script_filename.clear();
    _map_hud_name.clear();

    // Clear global world map file
    if (_world_map_image) {
        delete _world_map_image;
        _world_map_image = 0;
    }

    // Clear out the time played, in case of a new game
    SystemManager->SetPlayTime(0, 0, 0);

    _shop_data.clear();

    // Show the minimap by default when available
    _show_minimap = true;

    _home_map.Clear();
}

bool GameGlobal::DoesEnemyExist(uint32_t enemy_id)
{
    if(enemy_id == 0)
        return false;

    ReadScriptDescriptor& enemy_data = GetEnemiesScript();

    if (!enemy_data.OpenTable(enemy_id))
        return false;

    enemy_data.CloseTable(); // enemy_id
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Event Group Functions
////////////////////////////////////////////////////////////////////////////////

bool GameGlobal::DoesEventExist(const std::string &group_name, const std::string &event_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end())
        return false;

    std::map<std::string, int32_t>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
    if(event_iter == group_iter->second->GetEvents().end())
        return false;

    return true;
}

void GameGlobal::AddNewEventGroup(const std::string &group_name)
{
    if(DoesEventGroupExist(group_name)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed because there was already an event group that existed for "
                                       << "the requested group name: " << group_name << std::endl;
        return;
    }

    GlobalEventGroup *geg = new GlobalEventGroup(group_name);
    _event_groups.insert(std::make_pair(group_name, geg));
}

GlobalEventGroup *GameGlobal::GetEventGroup(const std::string &group_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not find any event group by the requested name: " << group_name << std::endl;
        return nullptr;
    }
    return (group_iter->second);
}

int32_t GameGlobal::GetEventValue(const std::string &group_name, const std::string &event_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end())
        return 0;

    std::map<std::string, int32_t>::const_iterator event_iter = group_iter->second->GetEvents().find(event_name);
    if(event_iter == group_iter->second->GetEvents().end())
        return 0;

    return event_iter->second;
}

void GameGlobal::SetEventValue(const std::string &group_name, const std::string &event_name, int32_t event_value)
{
    GlobalEventGroup *geg = 0;
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end()) {
        geg = new GlobalEventGroup(group_name);
        _event_groups.insert(std::make_pair(group_name, geg));
    } else {
        geg = group_iter->second;
    }

    geg->SetEvent(event_name, event_value);
}

uint32_t GameGlobal::GetNumberEvents(const std::string &group_name) const
{
    std::map<std::string, GlobalEventGroup *>::const_iterator group_iter = _event_groups.find(group_name);
    if(group_iter == _event_groups.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "could not find any event group by the requested name: " << group_name << std::endl;
        return 0;
    }
    return group_iter->second->GetNumberEvents();
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Quests Functions
////////////////////////////////////////////////////////////////////////////////

static const QuestLogInfo _empty_quest_log_info;

const QuestLogInfo& GameGlobal::GetQuestInfo(const std::string &quest_id) const
{
    std::map<std::string, QuestLogInfo>::const_iterator itr = _quest_log_info.find(quest_id);
    if(itr == _quest_log_info.end())
        return _empty_quest_log_info;
    return itr->second;
}

void GameGlobal::SetShopData(const std::string& shop_id, const ShopData& shop_data)
{
    _shop_data[shop_id] = shop_data;
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Other Functions
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::SetMap(const std::string &map_data_filename,
                        const std::string &map_script_filename,
                        const std::string &map_image_filename,
                        const vt_utils::ustring &map_hud_name)
{
    _map_data_filename = map_data_filename;
    _map_script_filename = map_script_filename;

    if(!_map_image.Load(map_image_filename))
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to load map image: " << map_image_filename << std::endl;

    // Updates the map hud names info.
    _previous_map_hud_name = _map_hud_name;
    _map_hud_name = map_hud_name;
    _same_map_hud_name_as_previous = (MakeStandardString(_previous_map_hud_name) == MakeStandardString(_map_hud_name));
}

bool GameGlobal::NewGame()
{
    // Make sure no debug data is in the way.
    ClearAllData();
    return _global_script.RunScriptFunction("NewGame");
}

bool GameGlobal::AutoSave(const std::string& map_data_file, const std::string& map_script_file,
                          uint32_t stamina,
                          uint32_t x_position, uint32_t y_position)
{
    // Don't autosave when the save slot was not yet chosen
    if (GetGameSlotId() == std::numeric_limits<uint32_t>::max())
        return false;

    std::ostringstream filename;
    filename << GetUserDataPath() + "saved_game_" << GetGameSlotId() << "_autosave.lua";

    // Make the map location known globally to other code that may need to know this information
    std::string previous_map_data = _map_data_filename;
    std::string previous_map_script = _map_script_filename;

    // Set map data for the save file.
    _map_data_filename = map_data_file;
    _map_script_filename = map_script_file;
    _save_stamina = stamina;

    bool save_completed = SaveGame(filename.str(), GetGameSlotId(), x_position, y_position);

    // Restore previous map data
    _map_data_filename = previous_map_data;
    _map_script_filename = previous_map_script;

    return save_completed;
}

bool GameGlobal::SaveGame(const std::string& filename, uint32_t slot_id, uint32_t x_position, uint32_t y_position)
{
    if (slot_id >= SystemManager->GetGameSaveSlots())
        return false;

    WriteScriptDescriptor file;
    if(file.OpenFile(filename) == false) {
        return false;
    }

    // Open the save_game1 table
    file.WriteLine("save_game1 = {");

    // Save simple play data
    file.InsertNewLine();
    file.WriteLine("map_data_filename = \"" + _map_data_filename + "\",");
    file.WriteLine("map_script_filename = \"" + _map_script_filename + "\",");
    //! \note Coords are in map tiles
    file.WriteLine("location_x = " + NumberToString(x_position) + ",");
    file.WriteLine("location_y = " + NumberToString(y_position) + ",");
    file.WriteLine("play_hours = " + NumberToString(SystemManager->GetPlayHours()) + ",");
    file.WriteLine("play_minutes = " + NumberToString(SystemManager->GetPlayMinutes()) + ",");
    file.WriteLine("play_seconds = " + NumberToString(SystemManager->GetPlaySeconds()) + ",");
    file.WriteLine("drunes = " + NumberToString(_drunes) + ",");
    file.WriteLine("stamina = " + NumberToString(_save_stamina) + ",");

    // Save latest home map data, if any.
    if (_home_map.IsValid()) {
        file.InsertNewLine();
        file.WriteLine("home_map = {");
        file.WriteLine("\tmap_data_filename = \"" + _home_map.GetMapDataFilename() + "\",");
        file.WriteLine("\tmap_script_filename = \"" + _home_map.GetMapDataFilename() + "\",");
        //! \note Coords are in map tiles
        file.WriteLine("\tlocation_x = " + NumberToString(_home_map.GetMapPosition().x) + ",");
        file.WriteLine("\tlocation_y = " + NumberToString(_home_map.GetMapPosition().y) + ",");
        file.WriteLine("},");
    }

    _inventory_handler.SaveInventory(file);

    _character_handler.SaveCharacters(file);

    // ----- (6) Save event data
    file.InsertNewLine();
    file.WriteLine("event_groups = {");
    for(std::map<std::string, GlobalEventGroup *>::iterator it = _event_groups.begin(); it != _event_groups.end(); ++it) {
        _SaveEvents(file, it->second);
    }
    file.WriteLine("},");
    file.InsertNewLine();

    // ------ (7) Save quest log
    file.WriteLine("quest_log = {");
    for(std::map<std::string, QuestLogEntry *>::const_iterator itr = _quest_log_entries.begin(); itr != _quest_log_entries.end(); ++itr)
        _SaveQuests(file, itr->second);
    file.WriteLine("},");
    file.InsertNewLine();

    _SaveWorldMap(file);

    _SaveShopData(file);

    if(file.IsErrorDetected()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "One or more errors occurred while writing the save game file - they are listed below:" << std::endl
            << file.GetErrorMessages() << std::endl;
        file.ClearErrors();
    }

    file.InsertNewLine();
    file.WriteLine("} -- save_game1"); //save_game1

    file.CloseFile();

    // Store the game slot the game is coming from.
    _game_slot_id = slot_id;

    return true;
}

bool GameGlobal::LoadGame(const std::string &filename, uint32_t slot_id)
{
    ReadScriptDescriptor file;
    if(!file.OpenFile(filename))
        return false;

    ClearAllData();

    // open the namespace that the save game is encapsulated in.
    if (!file.OpenTable("save_game1")) {
        PRINT_ERROR << "Couldn't open the savegame " << filename << std::endl;
        return false;
    }

    // Load play data
    // DEPRECATED: Old way to load, will be removed in a release
    if (file.DoesStringExist("map_filename")) {
        _map_data_filename = file.ReadString("map_filename");
        _map_script_filename = file.ReadString("map_filename");
    }
    else {
        // New way: data and script are separated.
        _map_data_filename = file.ReadString("map_data_filename");
        _map_script_filename = file.ReadString("map_script_filename");
    }

    // DEPRECATED: Remove in one release
    // Hack to permit the split of last map data and scripts.
    if (!_map_data_filename.empty() && _map_data_filename == _map_script_filename) {
        std::string map_common_name = _map_data_filename.substr(0, _map_data_filename.length() - 4);
        _map_data_filename = map_common_name + "_map.lua";
        _map_script_filename = map_common_name + "_script.lua";
    }

    // DEPRECATED: Remove in one release
    // test whether the beginning of the filepath is 'dat/maps/' and replace with 'data/story/'
    if (_map_data_filename.substr(0, 9) == "dat/maps/")
        _map_data_filename = std::string("data/story/") + _map_data_filename.substr(9, _map_data_filename.length() - 9);
    if (_map_script_filename.substr(0, 9) == "dat/maps/")
        _map_script_filename = std::string("data/story/") + _map_script_filename.substr(9, _map_script_filename.length() - 9);

    // Load a potential saved position
    _x_save_map_position = file.ReadUInt("location_x");
    _y_save_map_position = file.ReadUInt("location_y");
    uint8_t hours, minutes, seconds;
    hours = file.ReadUInt("play_hours");
    minutes = file.ReadUInt("play_minutes");
    seconds = file.ReadUInt("play_seconds");
    SystemManager->SetPlayTime(hours, minutes, seconds);
    _drunes = file.ReadUInt("drunes");
    if (file.DoesUIntExist("stamina"))
        _save_stamina = file.ReadUInt("stamina");

    // Load home map data, if any
    if (file.OpenTable("home_map")) {
        std::string home_map_data = file.ReadString("map_data_filename");
        std::string home_map_script = file.ReadString("map_script_filename");
        uint32_t x_pos = file.ReadUInt("location_x");
        uint32_t y_pos = file.ReadUInt("location_y");

        _home_map = vt_map::MapLocation(home_map_data,
                                        home_map_script,
                                        x_pos, y_pos);

        file.CloseTable(); // home_map
    }

    _inventory_handler.LoadInventory(file);

    _character_handler.LoadCharacters(file);

    // Load event data
    std::vector<std::string> group_names;
    if (file.OpenTable("event_groups")) {
        file.ReadTableKeys(group_names);
        for(uint32_t i = 0; i < group_names.size(); i++)
            _LoadEvents(file, group_names[i]);
        file.CloseTable();
    }

    // Load the quest log data
    std::vector<std::string> quest_keys;
    if (file.OpenTable("quest_log")) {
        file.ReadTableKeys(quest_keys);
        for(uint32_t i = 0; i < quest_keys.size(); ++i)
            _LoadQuests(file, quest_keys[i]);
        file.CloseTable();
    }

    // Load the world map data
    _LoadWorldMap(file);

    // Report any errors detected from the previous read operations
    if(file.IsErrorDetected()) {
        if(GLOBAL_DEBUG) {
            PRINT_WARNING << "one or more errors occurred while reading the save game file - they are listed below" << std::endl;
            std::cerr << file.GetErrorMessages() << std::endl;
            file.ClearErrors();
        }
    }

    _LoadShopData(file);

    file.CloseFile();

    // Store the game slot the game is coming from.
    _game_slot_id = slot_id;

    return true;
}

void GameGlobal::LoadEmotes(const std::string &emotes_filename)
{
    // First, clear the list in case of reloading
    _emotes.clear();

    vt_script::ReadScriptDescriptor emotes_script;
    if(!emotes_script.OpenFile(emotes_filename))
        return;

    if(!emotes_script.DoesTableExist("emotes")) {
        emotes_script.CloseFile();
        return;
    }

    std::vector<std::string> emotes_id;
    emotes_script.ReadTableKeys("emotes", emotes_id);

    // Read all the values
    emotes_script.OpenTable("emotes");
    for(uint32_t i = 0; i < emotes_id.size(); ++i) {

        if(!emotes_script.DoesTableExist(emotes_id[i]))
            continue;
        emotes_script.OpenTable(emotes_id[i]);

        std::string animation_file = emotes_script.ReadString("animation");

        AnimatedImage anim;
        if(anim.LoadFromAnimationScript(animation_file)) {
            // NOTE: The map mode should one day be fixed to use the same coords
            // than everything else, thus making possible to remove this
            vt_map::MapMode::ScaleToMapZoomRatio(anim);

            _emotes.insert(std::make_pair(emotes_id[i], anim));

            // The vector containing the offsets
            std::vector<std::pair<float, float> > emote_offsets;
            emote_offsets.resize(vt_map::private_map::NUM_ANIM_DIRECTIONS);

            // For each directions
            for(uint32_t j = 0; j < vt_map::private_map::NUM_ANIM_DIRECTIONS; ++j) {
                emotes_script.OpenTable(j);

                std::pair<float, float> offsets;
                offsets.first = emotes_script.ReadFloat("x");
                offsets.second = emotes_script.ReadFloat("y");

                emote_offsets[j] = offsets;

                emotes_script.CloseTable(); // direction table.
            }

            _emotes_offsets.insert(std::make_pair(emotes_id[i], emote_offsets));
        }

        emotes_script.CloseTable(); // emote_id[i]
    }
    emotes_script.CloseAllTables();
    emotes_script.CloseFile();
}

void GameGlobal::GetEmoteOffset(float &x, float &y, const std::string &emote_id, vt_map::private_map::ANIM_DIRECTIONS dir)
{
    x = 0.0f;
    y = 0.0f;

    if(dir < vt_map::private_map::ANIM_SOUTH || dir >= vt_map::private_map::NUM_ANIM_DIRECTIONS)
        return;

    std::map<std::string, std::vector<std::pair<float, float> > >::const_iterator it =
        _emotes_offsets.find(emote_id);

    if(it == _emotes_offsets.end())
        return;

    x = it->second[dir].first;
    y = it->second[dir].second;
}

////////////////////////////////////////////////////////////////////////////////
// GameGlobal class - Private Methods
////////////////////////////////////////////////////////////////////////////////

void GameGlobal::_SaveEvents(WriteScriptDescriptor &file, GlobalEventGroup *event_group)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }
    if(event_group == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a nullptr event group pointer argument" << std::endl;
        return;
    }

    file.WriteLine("\t" + event_group->GetGroupName() + " = {");

    uint32_t i = 0;
    for(std::map<std::string, int32_t>::const_iterator it = event_group->GetEvents().begin(); it != event_group->GetEvents().end(); ++it) {
        if(it == event_group->GetEvents().begin())
            file.WriteLine("\t\t", false);
        else
            file.WriteLine(", ", false);

        // Add a new line every 4 entries for better readability and debugging
        if ((i > 0) && !(i % 4)) {
            file.InsertNewLine();
            file.WriteLine("\t\t", false);
        }

        file.WriteLine("[\"" + it->first + "\"] = " + NumberToString(it->second), false);

        ++i;
    }
    file.WriteLine("\n\t},");
}

void GameGlobal::_SaveQuests(WriteScriptDescriptor &file, const QuestLogEntry *quest_log_entry)
{
    if(file.IsFileOpen() == false)
    {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    if(quest_log_entry == nullptr)
    {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "_SaveQuests function received a nullptr quest log entry pointer argument" << std::endl;
        return;
    }

    // Start writting
    file.WriteLine("\t" + quest_log_entry->GetQuestId() + " = {", false);
    // Write the quest log number. this is written as a string because loading needs a uniform type of data in the array
    file.WriteLine("\"" + NumberToString(quest_log_entry->GetQuestLogNumber()) + "\", ", false);
    // Write the "false" or "true" string if this entry has been read or not
    const std::string is_read(quest_log_entry->IsRead() ? "true" : "false");
    file.WriteLine("\"" + is_read + "\"", false);
    // End writing
    file.WriteLine("},");
}

void GameGlobal::_SaveWorldMap(vt_script::WriteScriptDescriptor &file)
{
    if(!file.IsFileOpen()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    // Write the 'worldmap' table
    file.WriteLine("worldmap = {");

    //write the world map filename
    file.WriteLine("\tworld_map_file = \"" + GetWorldMapFilename() + "\",");
    file.InsertNewLine();

    //write the viewable locations
    file.WriteLine("\tviewable_locations = {");
    for(uint32_t i = 0; i < _viewable_world_locations.size(); ++i)
        file.WriteLine("\t\t\"" + _viewable_world_locations[i]+"\",");
    file.WriteLine("\t},");
    file.InsertNewLine();

    file.WriteLine("\tcurrent_location = \"" + GetCurrentLocationId() + "\"");

    file.WriteLine("},"); // close the main table
    file.InsertNewLine();
}

void GameGlobal::_SaveShopData(vt_script::WriteScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "The file was not open: " << file.GetFilename() << std::endl;
        return;
    }

    file.WriteLine("shop_data = {");
    file.InsertNewLine();

    auto it = _shop_data.begin();
    auto it_end = _shop_data.end();
    for (; it != it_end; ++it) {
        std::string shop_id = it->first;
        const ShopData& shop_data = it->second;

        file.WriteLine("\t[\"" + shop_id + "\"] = {");

        file.WriteLine("\t\tavailable_buy = {");
        auto it2 = shop_data._available_buy.begin();
        auto it2_end = shop_data._available_buy.end();
        for(; it2 != it2_end; ++it2) {
            std::string item_id = NumberToString(it2->first);
            std::string count = NumberToString(it2->second);
            file.WriteLine("\t\t\t[\"" + item_id + "\"] = " + count + ",");
        }
        file.WriteLine("\t\t},");

        file.WriteLine("\t\tavailable_trade = {");
        auto it3 = shop_data._available_trade.begin();
        auto it3_end = shop_data._available_trade.end();
        for(; it3 != it3_end; ++it3) {
            std::string item_id = NumberToString(it3->first);
            std::string count = NumberToString(it3->second);
            file.WriteLine("\t\t\t[\"" + item_id + "\"] = " + count + ",");
        }
        file.WriteLine("\t\t}");

        file.WriteLine("\t},");
    }
    file.WriteLine("},"); // Close the shop_data table
    file.InsertNewLine();
}

void GameGlobal::_LoadEvents(ReadScriptDescriptor &file, const std::string &group_name)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    AddNewEventGroup(group_name);
    GlobalEventGroup *new_group = GetEventGroup(group_name); // new_group is guaranteed not to be nullptr

    std::vector<std::string> event_names;

    if (file.OpenTable(group_name)) {
        file.ReadTableKeys(event_names);
        for(uint32_t i = 0; i < event_names.size(); i++) {
            new_group->AddNewEvent(event_names[i], file.ReadInt(event_names[i]));
        }
        file.CloseTable();
    }
    else {
        PRINT_ERROR << "Invalid event group name '" << group_name << " in save file "
                << file.GetFilename() << std::endl;
    }
}

void GameGlobal::_LoadQuests(ReadScriptDescriptor &file, const std::string &quest_id)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }
    std::vector<std::string> quest_info;
    //read the 4 entries into a new quest entry
    file.ReadStringVector(quest_id, quest_info);
    if(quest_info.size() != 2)
    {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "save file has malformed quest log entries" << std::endl;
        return;
    }

    //conversion of the log number from string int. We need to do thing because ReadStringVector assumes that
    //all items are the same type.
    uint32_t quest_log_number = std::stoi(quest_info[0]);
    //conversion from string to bool for is_read flag
    bool is_read = quest_info[1].compare("true") == 0;

    if(!_AddQuestLog(quest_id, quest_log_number, is_read))
    {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "save file has duplicate quest log id entries" << std::endl;
        return;
    }
    //update the quest log count value if the current number is greater
    if(_quest_log_count < quest_log_number)
        _quest_log_count = quest_log_number;
}

void GameGlobal::_LoadWorldMap(vt_script::ReadScriptDescriptor &file)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "the file provided in the function argument was not open" << std::endl;
        return;
    }

    if (!file.OpenTable("worldmap")) {
        // DEPRECATED! Old World map format. Removed in one release...
        std::string world_map = file.ReadString("world_map");
        SetWorldMap(world_map);

        std::vector<std::string> location_ids;
        file.ReadStringVector("viewable_locations", location_ids);
        for(uint32_t i = 0; i < location_ids.size(); ++i)
            ShowWorldLocation(location_ids[i]);
        return;
    }

    std::string world_map = file.ReadString("world_map_file");

    // DEPRECATED: Remove this hack in one release
    if (world_map.substr(0, 20) == "img/menus/worldmaps/")
        world_map = std::string("data/story/common/worldmaps/") + world_map.substr(20, world_map.length() - 20);

    SetWorldMap(world_map);

    std::vector<std::string> location_ids;
    file.ReadStringVector("viewable_locations", location_ids);
    for(uint32_t i = 0; i < location_ids.size(); ++i)
        ShowWorldLocation(location_ids[i]);

    std::string current_location = file.ReadString("current_location");
    if (!current_location.empty())
        SetCurrentLocationId(current_location);

    file.CloseTable(); // worldmap
}

bool GameGlobal::_LoadWorldLocationsScript(const std::string &world_locations_filename)
{
    _world_map_locations.clear();

    vt_script::ReadScriptDescriptor world_locations_script;
    if(!world_locations_script.OpenFile(world_locations_filename)) {
        PRINT_ERROR << "Couldn't open world map locations file: " << world_locations_filename << std::endl;
        return false;
    }

    if(!world_locations_script.DoesTableExist("world_locations"))
    {
        PRINT_ERROR << "No 'world_locations' table in file: " << world_locations_filename << std::endl;
        world_locations_script.CloseFile();
        return false;
    }

    std::vector<std::string> world_location_ids;
    world_locations_script.ReadTableKeys("world_locations", world_location_ids);
    if(world_location_ids.empty())
    {
        PRINT_ERROR << "No items in 'world_locations' table in file: " << world_locations_filename << std::endl;
        world_locations_script.CloseFile();
        return false;
    }

    world_locations_script.OpenTable("world_locations");
    for(uint32_t i = 0; i < world_location_ids.size(); ++i)
    {
        const std::string &id = world_location_ids[i];
        std::vector<std::string> values;
        world_locations_script.ReadStringVector(id,values);

        //check for existing location
        if(_world_map_locations.find(id) != _world_map_locations.end())
        {
            PRINT_WARNING << "duplicate world map location id found: " << id << std::endl;
            continue;
        }

        float x = atof(values[0].c_str());
        float y = atof(values[1].c_str());
        const std::string &location_name = values[2];
        const std::string &image_path = values[3];
        WorldMapLocation location(x, y, location_name, image_path, id);

        _world_map_locations[id] = location;

    }
    return true;

}

//! (Re)Loads the quest entries into the GlobalManager
bool GameGlobal::_LoadQuestsScript(const std::string& quests_script_filename)
{
    // First clear the existing quests entries in case of a reloading.
    _quest_log_info.clear();

    vt_script::ReadScriptDescriptor quests_script;
    if(!quests_script.OpenFile(quests_script_filename)) {
        PRINT_ERROR << "Couldn't open quests file: " << quests_script_filename
                    << std::endl;
        return false;
    }

    if(!quests_script.DoesTableExist("quests")) {
        PRINT_ERROR << "No 'quests' table in file: " << quests_script_filename
                    << std::endl;
        quests_script.CloseFile();
        return false;
    }

    std::vector<std::string> quest_ids;
    quests_script.ReadTableKeys("quests", quest_ids);
    if(quest_ids.empty()) {
        PRINT_ERROR << "No quests defined in the 'quests' table of file: "
                    << quests_script_filename << std::endl;
        quests_script.CloseFile();
        return false;
    }

    quests_script.OpenTable("quests");
    for(uint32_t i = 0; i < quest_ids.size(); ++i)
    {
        const std::string& quest_id = quest_ids[i];
        std::vector<std::string> quest_info;

        quests_script.ReadStringVector(quest_id, quest_info);

        // Check for an existing quest entry
        if(_quest_log_info.find(quest_id) != _quest_log_info.end()) {
            PRINT_WARNING << "Duplicate quests defined in the 'quests' table of file: "
                << quests_script_filename << std::endl;
            continue;
        }

        //check whether all fields are there.
        if(quest_info.size() >= 9)
        {
            QuestLogInfo info = QuestLogInfo(MakeUnicodeString(quest_info[0]),
                                     MakeUnicodeString(quest_info[1]),
                                     MakeUnicodeString(quest_info[2]),
                                     quest_info[3], quest_info[4],
                                     MakeUnicodeString(quest_info[5]), quest_info[6],
                                     MakeUnicodeString(quest_info[7]), quest_info[8]);
            // If possible, loads the non-competable event group and name
            if (quest_info.size() == 11) {
                info.SetNotCompletableIf(quest_info[9], quest_info[10]);
            }
            _quest_log_info[quest_id] = info;
        }
        //malformed quest log
        else
        {
            PRINT_ERROR << "malformed quest log for id: " << quest_id << std::endl;
        }
    }

    quests_script.CloseTable();

    quests_script.CloseFile();

    return true;
}

void GameGlobal::_LoadShopData(vt_script::ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    if (!file.OpenTable("shop_data")) {
        return;
    }

    std::vector<std::string> shop_ids;
    file.ReadTableKeys(shop_ids);

    for (size_t i = 0; i < shop_ids.size(); ++i) {
        // Open the Shop Id table
        if (!file.OpenTable(shop_ids[i]))
            continue;

        ShopData shop_data;
        if (file.OpenTable("available_buy")) {
            std::vector<std::string> item_ids;
            file.ReadTableKeys(item_ids);
            for (size_t j = 0; j < item_ids.size(); ++j) {
                uint32_t item_count = file.ReadUInt(item_ids[j]);
                shop_data._available_buy[std::stoi(item_ids[j].c_str())] = item_count;
            }
            file.CloseTable(); // available_buy
        }
        if (file.OpenTable("available_trade")) {
            std::vector<std::string> item_ids;
            file.ReadTableKeys(item_ids);
            for (size_t j = 0; j < item_ids.size(); ++j) {
                uint32_t item_count = file.ReadUInt(item_ids[j]);
                shop_data._available_trade[std::stoi(item_ids[j].c_str())] = item_count;
            }
            file.CloseTable(); // available_trade
        }
        _shop_data[ shop_ids[i] ] = shop_data;
        file.CloseTable(); // shop_id
    }
    file.CloseTable(); // shop_data
}

} // namespace vt_global
