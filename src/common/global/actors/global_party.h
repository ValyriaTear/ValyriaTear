////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_PARTY_HEADER__
#define __GLOBAL_PARTY_HEADER__

#include "utils/utils_common.h"

#include <vector>

namespace vt_global
{

class GlobalCharacter;

//! \brief The maximum number of characters that can be in the active party
const uint32_t GLOBAL_MAX_PARTY_SIZE = 4;

/** ****************************************************************************
*** \brief Represents a party of characters
***
*** This class is a container for a group or "party" of characters. A party is a type
*** of target for items and skills. The GameGlobal class also organizes characters
*** into parties for convienence. Note that an character may be either an enemy or
*** a character, but you should avoid creating parties that contain both characters
*** and enemies, as it can lead to conflicts. For example, a character and enemy which
*** enemy which have the same ID value).
***
*** Parties may or may not allow duplicate characters (a duplicate character is defined
*** as an character that has the same _id member as another character in the party).
*** This property is determined in the GlobalParty constructor
***
*** \note When this class is destroyed, the characters contained within the class are
*** <i>not</i> destroyed. Only the references to those characters through this class object
*** are lost.
***
*** \note All methods which perform an operation by using an character ID are
*** <b>only</b> valid to use if the party does not allow duplicates.
*** ***************************************************************************/
class GlobalParty
{
public:
    //! \param allow_duplicates Determines whether or not the party allows duplicate characters to be added (default value == false)
    explicit GlobalParty(bool allow_duplicates = false) :
        _allow_duplicates(allow_duplicates)
    {}

    ~GlobalParty()
    {}

    // ---------- Character addition, removal, and retrieval methods

    /** \brief Adds an character to the party
    *** \param character A pointer to the character to add to the party
    *** \param index The index where the character should be inserted. If negative, character is added to the end
    *** \note The character will not be added if it is already in the party and duplicates are not allowed
    **/
    void AddCharacter(GlobalCharacter *character, int32_t index = -1);

    /** \brief Removes an character from the party
    *** \param index The index of the character in the party to remove
    *** \return A pointer to the character that was removed, or nullptr if the index provided was invalid
    **/
    GlobalCharacter *RemoveCharacterAtIndex(uint32_t index);

    /** \brief Removes an character from the party
    *** \param id The id value of the character to remove
    *** \return A pointer to the character that was removed, or nullptr if the character was not found in the party
    **/
    GlobalCharacter *RemoveCharacterByID(uint32_t id);

    /** \brief Clears the party of all characters
    *** \note This function does not return the character pointers, so if you wish to get the
    *** GlobalCharacter make sure you do so prior to invoking this call.
    **/
    void RemoveAllCharacters() {
        _characters.clear();
    }

    /** \brief Retrieves a poitner to the character in the party at a specified index
    *** \param index The index where the character may be found in the party
    *** \return A pointer to the character at the specified index, or nullptr if the index argument was invalid
    **/
    GlobalCharacter *GetCharacterAtIndex(uint32_t index) const;

    /** \brief Retrieves a poitner to the character in the party with the spefified id
    *** \param id The id of the character to return
    *** \return A pointer to the character with the requested ID, or nullptr if the character was not found
    **/
    GlobalCharacter *GetCharacterByID(uint32_t id) const;

    // ---------- Character swap and replacement methods

    /** \brief Swaps the location of two characters in the party by their indeces
    *** \param first_index The index of the first character to swap
    *** \param second_index The index of the second character to swap
    **/
    void SwapCharactersByIndex(uint32_t first_index, uint32_t second_index);

    /** \brief Swaps the location of two characters in the party by looking up their IDs
    *** \param first_id The id of the first character to swap
    *** \param second_id The id of the second character to swap
    **/
    void SwapCharactersByID(uint32_t first_id, uint32_t second_id);

    /** \brief Replaces an character in the party at a specified index with a new character
    *** \param index The index of the character to be replaced
    *** \param new_character A pointer to the character that will replace the existing character
    *** \return A pointer to the replaced character, or nullptr if the operation did not take place
    **/
    GlobalCharacter *ReplaceCharacterByIndex(uint32_t index, GlobalCharacter *new_character);

    /** \brief Replaces an character in the party with the specified id with a new character
    *** \param id The id of the character to be replaced
    *** \param new_character A pointer to the character that will replace the existing character
    *** \return A pointer to the replaced character, or nullptr if the operation did not take place
    **/
    GlobalCharacter *ReplaceCharacterByID(uint32_t id, GlobalCharacter *new_character);

    // ---------- Other methods

    /** \brief Computes the average experience level of all characters in the party
    *** \return A float representing the average experience level (0.0f if party is empty)
    **/
    float AverageExperienceLevel() const;

    /** \brief Adds a certain amount of hit points to all characters in the party
    *** \param hp The number of health points to add
    **/
    void AddHitPoints(uint32_t hp);

    /** \brief Adds a certain amount of skill points to all characters in the party
    *** \param sp The number of skill points to add
    **/
    void AddSkillPoints(uint32_t sp);

    bool IsAllowDuplicates() const {
        return _allow_duplicates;
    }

    bool IsPartyEmpty() const {
        return _characters.empty();
    }

    uint32_t GetPartySize() const {
        return _characters.size();
    }

    const std::vector<GlobalCharacter *>& GetAllCharacters() const {
        return _characters;
    }

    //! \brief Returns the character party position
    uint32_t GetPartyPosition(GlobalCharacter* character);

private:
    /** \brief Characters are allowed to be inserted into the party multiple times when this member is true
    *** \note The value of this member is set in the class constructor and can not be changed at a later time
    **/
    bool _allow_duplicates;

    /** \brief A container of characters that are in this party
    *** The GlobalCharacter objects pointed to by the elements in this vector are not managed by this class. Therefore
    *** one needs to be careful that if any of the GlobalCharacter objects are destroyed outside the context of this
    *** class, the character should be removed from this container immediately to avoid a possible segmentation fault.
    **/
    std::vector<GlobalCharacter *> _characters;
}; // class GlobalParty

} // namespace vt_global

#endif // __GLOBAL_PARTY_HEADER__
