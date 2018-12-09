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
    _max_experience_level(100)
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

    if(!_game_quests.LoadQuestsScript("data/config/quests.lua"))
        return false;

    if(!_worldmap_handler.LoadScript("data/config/world_locations.lua"))
        return false;

    if (!_skill_graph.Initialize("data/config/skill_graph.lua"))
        return false;

    return true;
}

void GameGlobal::ClearAllData()
{
    _inventory_handler.ClearAllData();

    _character_handler.ClearAllData();

    _game_events.Clear();

    _game_quests.Clear();

    _map_data_handler.Clear();

    _worldmap_handler.ClearWorldMapImage();

    // Clear out the time played, in case of a new game
    SystemManager->SetPlayTime(0, 0, 0);

    _shop_data.clear();
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

void GameGlobal::SetShopData(const std::string& shop_id, const ShopData& shop_data)
{
    _shop_data[shop_id] = shop_data;
}

bool GameGlobal::NewGame()
{
    // Make sure no debug data is in the way.
    ClearAllData();
    return _global_script.RunScriptFunction("NewGame");
}

bool GameGlobal::AutoSave(const std::string& map_data_file,
                          const std::string& map_script_file,
                          uint32_t stamina,
                          uint32_t x_position, uint32_t y_position)
{
    // Don't autosave when the save slot was not yet chosen
    if (GetGameSlotId() == std::numeric_limits<uint32_t>::max())
        return false;

    std::ostringstream filename;
    filename << GetUserDataPath() + "saved_game_" << GetGameSlotId() << "_autosave.lua";

    // Make the map location known globally to other code that may need to know this information
    std::string previous_map_data = _map_data_handler.GetMapDataFilename();
    std::string previous_map_script = _map_data_handler.GetMapScriptFilename();

    // Set map data for the save file.
    _map_data_handler.SetMapDataFilename(map_data_file);
    _map_data_handler.SetMapScriptFilename(map_script_file);
    _map_data_handler.SetSaveStamina(stamina);

    bool save_completed = SaveGame(filename.str(), GetGameSlotId(), x_position, y_position);

    // Restore previous map data
    _map_data_handler.SetMapDataFilename(previous_map_data);
    _map_data_handler.SetMapScriptFilename(previous_map_script);

    return save_completed;
}

bool GameGlobal::SaveGame(const std::string& filename,
                          uint32_t slot_id,
                          uint32_t x_position,
                          uint32_t y_position)
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
    file.WriteLine("play_hours = " + NumberToString(SystemManager->GetPlayHours()) + ",");
    file.WriteLine("play_minutes = " + NumberToString(SystemManager->GetPlayMinutes()) + ",");
    file.WriteLine("play_seconds = " + NumberToString(SystemManager->GetPlaySeconds()) + ",");
    file.WriteLine("drunes = " + NumberToString(_drunes) + ",");

    _map_data_handler.Save(file, x_position, y_position);

    _inventory_handler.SaveInventory(file);

    _character_handler.SaveCharacters(file);

    _game_events.SaveEvents(file);

    _game_quests.SaveQuests(file);

    _worldmap_handler.SaveWorldMap(file);

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

    _map_data_handler.Load(file);

    uint8_t hours, minutes, seconds;
    hours = file.ReadUInt("play_hours");
    minutes = file.ReadUInt("play_minutes");
    seconds = file.ReadUInt("play_seconds");
    SystemManager->SetPlayTime(hours, minutes, seconds);
    _drunes = file.ReadUInt("drunes");

    _inventory_handler.LoadInventory(file);

    _character_handler.LoadCharacters(file);

    _game_events.LoadEvents(file);

    _game_quests.LoadQuests(file);

    // Load the world map data
    _worldmap_handler.LoadWorldMap(file);

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
