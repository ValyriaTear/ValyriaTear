////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_character_handler.h"

#include "common/global/actors/global_character.h"

using namespace vt_utils;

namespace vt_global
{

void CharacterHandler::AddCharacter(uint32_t id)
{
    std::map<uint32_t, GlobalCharacter *>::iterator it = _characters.find(id);
    if(it != _characters.end()) {
        if(it->second->IsEnabled()) {
            PRINT_WARNING << "attempted to add a character that already existed: " << id << std::endl;
            return;
        } else {
            // Re-enable the character in that case
            it->second->Enable(true);
        }
    }

    GlobalCharacter* character = nullptr;

    // Add the character if not existing in the main character data
    if(it == _characters.end()) {
        character = new GlobalCharacter(id);

        _characters.insert(std::make_pair(id, character));
    } else {
        character = it->second;
    }

    // Add the new character to the active party if the active party contains less than four characters
    if(_ordered_characters.size() < GLOBAL_MAX_PARTY_SIZE)
        _active_party.AddCharacter(character);

    _ordered_characters.push_back(character);
}

void CharacterHandler::AddCharacter(GlobalCharacter* ch)
{
    if(ch == nullptr) {
        PRINT_WARNING << "function received nullptr pointer argument" << std::endl;
        return;
    }

    if(_characters.find(ch->GetID()) != _characters.end()) {
        PRINT_WARNING << "attempted to add a character that already existed: " << ch->GetID() << std::endl;
        return;
    }

    _characters.insert(std::make_pair(ch->GetID(), ch));

    // If the charactger is currently disabled, don't add it to the available characters.
    if(!ch->IsEnabled())
        return;

    // Add the new character to the active party if the active party contains less than four characters
    if(_ordered_characters.size() < GLOBAL_MAX_PARTY_SIZE)
        _active_party.AddCharacter(ch);

    _ordered_characters.push_back(ch);
}

void CharacterHandler::RemoveCharacter(uint32_t id, bool erase)
{
    std::map<uint32_t, GlobalCharacter *>::iterator it = _characters.find(id);
    if(it == _characters.end()) {
        PRINT_WARNING << "attempted to remove a character that did not exist: " << id << std::endl;
        return;
    }

    // Disable the character
    it->second->Enable(false);

    for(std::vector<GlobalCharacter *>::iterator it = _ordered_characters.begin(); it != _ordered_characters.end(); ++it) {
        if((*it)->GetID() == id) {
            _ordered_characters.erase(it);
            break;
        }
    }

    // Reform the active party in case the removed character was a member of it
    _active_party.RemoveAllCharacters();
    for(uint32_t j = 0; j < _ordered_characters.size() && j < GLOBAL_MAX_PARTY_SIZE; j++) {
        _active_party.AddCharacter(_ordered_characters[j]);
    }

    // If we were asked to remove the character completely from the game data.
    if(erase) {
        delete(it->second);
        _characters.erase(it);
    }
}

GlobalCharacter* CharacterHandler::GetCharacter(uint32_t id)
{
    std::map<uint32_t, GlobalCharacter *>::iterator ch = _characters.find(id);
    if(ch == _characters.end())
        return nullptr;
    else
        return (ch->second);
}

void CharacterHandler::SwapCharactersByIndex(uint32_t first_index, uint32_t second_index)
{
    // Deal with the ordered characters
    if(first_index == second_index) {
        PRINT_WARNING << "first_index and second_index arguments had the same value: " << first_index << std::endl;
        return;
    }
    if(first_index >= _ordered_characters.size()) {
        PRINT_WARNING << "first_index argument exceeded current party size: " << first_index << std::endl;
        return;
    }
    if(second_index >= _ordered_characters.size()) {
        PRINT_WARNING << "second_index argument exceeded current party size: " << second_index << std::endl;
        return;
    }

    // Swap the characters
    GlobalCharacter *tmp = _ordered_characters[first_index];
    _ordered_characters[first_index] = _ordered_characters[second_index];
    _ordered_characters[second_index] = tmp;

    // Do the same for the party member.
    _active_party.SwapCharactersByIndex(first_index, second_index);
}

void CharacterHandler::ClearAllData()
{
    for(std::map<uint32_t, GlobalCharacter *>::iterator it = _characters.begin(); it != _characters.end(); ++it) {
        delete it->second;
    }
    _characters.clear();
    _ordered_characters.clear();
    _active_party.RemoveAllCharacters();
}

bool CharacterHandler::LoadCharacters(vt_script::ReadScriptDescriptor& file)
{
    // Load characters into the party in the correct order
    if (!file.OpenTable("characters")) {
        PRINT_ERROR << "Couldn't open the savegame characters data in " << file.GetFilename() << std::endl;
        file.CloseAllTables();
        file.CloseFile();
        return false;
    }

    if (!file.DoesTableExist("order")) {
        PRINT_ERROR << "Couldn't open the savegame characters order data in " << file.GetFilename() << std::endl;
        file.CloseAllTables();
        file.CloseFile();
        return false;
    }

    std::vector<uint32_t> char_ids;
    file.ReadUIntVector("order", char_ids);

    if (char_ids.empty()) {
        PRINT_ERROR << "No valid characters id in " << file.GetFilename() << std::endl;
        file.CloseAllTables();
        file.CloseFile();
        return false;
    }

    for(uint32_t i = 0; i < char_ids.size(); ++i) {
        uint32_t id = char_ids[i];
        GlobalCharacter* character = new GlobalCharacter(id, false);
        if (character->LoadCharacter(file)) {
            AddCharacter(character);
        }
        else {
            delete character;
            PRINT_ERROR << "Invalid character id " << id << " in " << file.GetFilename() << std::endl;
            file.CloseAllTables();
            file.CloseFile();
            return false;
        }
    }
    file.CloseTable(); // characters

    if (_characters.empty()) {
        PRINT_ERROR << "No characters were added by save game file: " << file.GetFilename() << std::endl;
        file.CloseAllTables();
        file.CloseFile();
        return false;
    }
    return true;
}

void CharacterHandler::SaveCharacters(vt_script::WriteScriptDescriptor& file)
{
    file.InsertNewLine();
    file.WriteLine("characters = {");
    // First save the order of the characters in the party
    file.WriteLine("\t[\"order\"] = {");
    for(uint32_t i = 0; i < _ordered_characters.size(); ++i) {
        if(i == 0)
            file.WriteLine("\t\t" + NumberToString(_ordered_characters[i]->GetID()), false);
        else
            file.WriteLine(", " + NumberToString(_ordered_characters[i]->GetID()), false);
    }
    file.WriteLine("\n\t},"); // order

    // Now save each individual character's data
    for(uint32_t i = 0; i < _ordered_characters.size(); ++i) {
        _ordered_characters[i]->SaveCharacter(file);
    }
    file.WriteLine("},"); // characters
}

} // namespace vt_global
