////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_CHARACTER_HANDLER_HEADER__
#define __GLOBAL_CHARACTER_HANDLER_HEADER__

#include "global_party.h"

#include "script/script_read.h"
#include "script/script_write.h"

#include <map>

namespace vt_global
{

class GlobalCharacter;

/** ****************************************************************************
*** \brief Handles global character management, including adding them
*** in the active battle party, hidding them depending on the story, ...
*** ***************************************************************************/
class CharacterHandler
{
public:
    explicit CharacterHandler()
    {}

    ~CharacterHandler()
    {}

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
    void AddCharacter(uint32_t id);

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
    void RemoveCharacter(uint32_t id, bool erase = false);

    /** \brief Returns a pointer to a character currently in the party.
    *** \param id The ID number of the character to retrieve.
    *** \return A pointer to the character, or nullptr if the character was not found.
    ***/
    GlobalCharacter* GetCharacter(uint32_t id);

    /** \brief Swaps the location of two character in the party by their indeces
    *** \param first_index The index of the first character to swap
    *** \param second_index The index of the second character to swap
    **/
    void SwapCharactersByIndex(uint32_t first_index, uint32_t second_index);

    /** \brief Checks whether or not a character is in the party
    *** \param id The id of the character to check for
    *** \return True if the character was found to be in the party, or false if they were not found.
    **/
    bool IsCharacterAdded(uint32_t id) {
        if(_characters.find(id) != _characters.end()) return true;
        else return false;
    }

    std::vector<GlobalCharacter *>* GetOrderedCharacters() {
        return &_ordered_characters;
    }

    GlobalParty& GetActiveParty() {
        return _active_party;
    }

    //! \brief Resets the data. Used in new games
    void ClearAllData();

    bool LoadCharacters(vt_script::ReadScriptDescriptor& file);
    void SaveCharacters(vt_script::WriteScriptDescriptor& file);

private:
    /** \brief A map containing all characters that the player has discovered
    *** This map contains all characters that the player has met with, regardless of whether or not they are in the active party.
    *** The map key is the character's unique ID number.
    **/
    std::map<uint32_t, GlobalCharacter *> _characters;

    /** \brief A vector whose purpose is to maintain the order of characters
    *** The first four characters in this vector are in the active party; the rest are in reserve.
    **/
    std::vector<GlobalCharacter *> _ordered_characters;

    /** \brief The active party of characters
    *** The active party contains the group of characters that will fight when a battle begins.
    *** This party can be up to four characters, and should always contain at least one character.
    **/
    GlobalParty _active_party;
};

} // namespace vt_global

#endif // __GLOBAL_CHARACTER_HANDLER_HEADER__
