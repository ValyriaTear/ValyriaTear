////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "global_party.h"

#include "global_character.h"

namespace vt_global
{

extern bool GLOBAL_DEBUG;

void GlobalParty::AddCharacter(GlobalCharacter* character, int32_t index)
{
    if(character == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a nullptr character argument" << std::endl;
        return;
    }

    if(_allow_duplicates == false) {
        // Check that this character is not already in the party
        for(uint32_t i = 0; i < _characters.size(); ++i) {
            if(character->GetID() == _characters[i]->GetID()) {
                IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to add an character that was already in the party "
                                               << "when duplicates were not allowed: " << character->GetID() << std::endl;
                return;
            }
        }
    }

    // Add character to the end of the party if index is negative
    if(index < 0) {
        _characters.push_back(character);
        return;
    }

    // Check that the requested index does not exceed the size of the container
    if(static_cast<uint32_t>(index) >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded the current party size: " << index << std::endl;
        _characters.push_back(character); // Add the character to the end of the party instead
        return;
    } else {
        std::vector<GlobalCharacter *>::iterator position = _characters.begin();
        for(int32_t i = 0; i < index; ++i, ++position);
        _characters.insert(position, character);
    }
}

GlobalCharacter* GlobalParty::RemoveCharacterAtIndex(uint32_t index)
{
    if(index >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: "
                                       << index << std::endl;
        return nullptr;
    }

    GlobalCharacter *removed_character = _characters[index];
    std::vector<GlobalCharacter *>::iterator position = _characters.begin();
    for(uint32_t i = 0; i < index; ++i, ++position);
    _characters.erase(position);

    return removed_character;
}

GlobalCharacter *GlobalParty::RemoveCharacterByID(uint32_t id)
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to remove character when duplicates were allowed in the party: " << id << std::endl;
        return nullptr;
    }

    GlobalCharacter *removed_character = nullptr;
    for(std::vector<GlobalCharacter *>::iterator position = _characters.begin(); position != _characters.end(); ++position) {
        if(id == (*position)->GetID()) {
            removed_character = *position;
            _characters.erase(position);
            break;
        }
    }

    if(removed_character == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an character in the party with the requested id: " << id << std::endl;
    }

    return removed_character;
}

GlobalCharacter* GlobalParty::GetCharacterAtIndex(uint32_t index) const
{
    if(index >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << std::endl;
        return nullptr;
    }

    return _characters[index];
}

GlobalCharacter* GlobalParty::GetCharacterByID(uint32_t id) const
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to retrieve character when duplicates were allowed in the party: " << id << std::endl;
        return nullptr;
    }

    for(uint32_t i = 0; i < _characters.size(); ++i) {
        if(_characters[i]->GetID() == id) {
            return _characters[i];
        }
    }

    IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an character in the party with the requested id: " << id << std::endl;
    return nullptr;
}

void GlobalParty::SwapCharactersByIndex(uint32_t first_index, uint32_t second_index)
{
    if(first_index == second_index) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index and second_index arguments had the same value: " << first_index << std::endl;
        return;
    }
    if(first_index >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_index argument exceeded current party size: " << first_index << std::endl;
        return;
    }
    if(second_index >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "second_index argument exceeded current party size: " << second_index << std::endl;
        return;
    }

    GlobalCharacter* tmp = _characters[first_index];
    _characters[first_index] = _characters[second_index];
    _characters[second_index] = tmp;
}

void GlobalParty::SwapCharactersByID(uint32_t first_id, uint32_t second_id)
{
    if(first_id == second_id) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "first_id and second_id arguments had the same value: " << first_id << std::endl;
        return;
    }
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to swap characters when duplicates were allowed in the party: " << first_id << std::endl;
        return;
    }

    std::vector<GlobalCharacter *>::iterator first_position;
    std::vector<GlobalCharacter *>::iterator second_position;
    for(first_position = _characters.begin(); first_position != _characters.end(); ++first_position) {
        if((*first_position)->GetID() == first_id)
            break;
    }
    for(second_position = _characters.begin(); second_position != _characters.end(); ++second_position) {
        if((*second_position)->GetID() == second_id)
            break;
    }

    if(first_position == _characters.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an character in the party with the requested first_id: " << first_id << std::endl;
        return;
    }
    if(second_position == _characters.end()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an character in the party with the requested second_id: " << second_id << std::endl;
        return;
    }

    GlobalCharacter *tmp = *first_position;
    *first_position = *second_position;
    *second_position = tmp;
}

GlobalCharacter* GlobalParty::ReplaceCharacterByIndex(uint32_t index, GlobalCharacter* new_character)
{
    if(new_character == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a nullptr new_character argument" << std::endl;
        return nullptr;
    }
    if(index >= _characters.size()) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "index argument exceeded current party size: " << index << std::endl;
        return nullptr;
    }

    GlobalCharacter *tmp = _characters[index];
    _characters[index] = new_character;
    return tmp;
}

GlobalCharacter* GlobalParty::ReplaceCharacterByID(uint32_t id, GlobalCharacter* new_character)
{
    if(_allow_duplicates) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "tried to replace character when duplicates were allowed in the party: " << id << std::endl;
        return nullptr;
    }
    if(new_character == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received a nullptr new_character argument" << std::endl;
        return nullptr;
    }

    GlobalCharacter *removed_character = nullptr;
    for(std::vector<GlobalCharacter *>::iterator position = _characters.begin(); position != _characters.end(); ++position) {
        if((*position)->GetID() == id) {
            removed_character = *position;
            *position = new_character;
            break;
        }
    }

    if(removed_character == nullptr) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "failed to find an character in the party with the requested id: " << id << std::endl;
    }

    return removed_character;
}

float GlobalParty::AverageExperienceLevel() const
{
    if(_characters.empty())
        return 0.0f;

    float xp_level_sum = 0.0f;
    for(uint32_t i = 0; i < _characters.size(); ++i)
        xp_level_sum += static_cast<float>(_characters[i]->GetExperienceLevel());
    return (xp_level_sum / static_cast<float>(_characters.size()));
}

void GlobalParty::AddHitPoints(uint32_t hp)
{
    for(std::vector<GlobalCharacter *>::iterator i = _characters.begin(); i != _characters.end(); ++i) {
        (*i)->AddHitPoints(hp);
    }
}

void GlobalParty::AddSkillPoints(uint32_t sp)
{
    for(std::vector<GlobalCharacter *>::iterator i = _characters.begin(); i != _characters.end(); ++i) {
        (*i)->AddSkillPoints(sp);
    }
}

// Returns the character party position
uint32_t GlobalParty::GetPartyPosition(GlobalCharacter* character) {
    for (uint32_t i = 0; i < _characters.size(); ++i) {
        if (_characters[i] == character)
            return i;
    }
    // Default case
    return 0;
}

} // namespace vt_global
