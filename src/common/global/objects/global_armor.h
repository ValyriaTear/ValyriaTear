////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ARMOR_HEADER__
#define __GLOBAL_ARMOR_HEADER__

#include "global_object.h"

namespace vt_global
{

class GlobalSpirit;

/** ****************************************************************************
*** \brief Represents all types of armor that may be equipped on characters and enemies
***
*** There are actually four types of armor: head, torso, arm, and leg. However all
*** four types are represented by this single class. The only functional difference
*** between different types of armor is where they may be equipped on an actor. Not
*** all armor can be equipped by any character or enemy. Typically, armor may only
*** be used by a select few and can not be equipped on every character. Armor have
*** two defense ratings: physical and magical, both of which are included in
*** the damage calculation formulae when a character or enemy is attacked at the
*** location where the armor is equipped. Armor may also have a small number of
*** "sockets" in which spirits can be inserted to improve or alter the armor's
*** properties. Some armor will have zero sockets available. Finally, armor may
*** come imbued with certain elemental or status effect properties that bolster
*** and protect the user.
*** ***************************************************************************/
class GlobalArmor : public GlobalObject
{
public:
    explicit GlobalArmor(uint32_t id, uint32_t count = 1);
    virtual ~GlobalArmor() override
    {
    }

    //! \brief Returns the approriate armor type (head, torso, arm, leg) depending on the object ID
    GLOBAL_OBJECT GetObjectType() const override;

    uint32_t GetPhysicalDefense() const {
        return _physical_defense;
    }

    uint32_t GetMagicalDefense() const {
        return _magical_defense;
    }

    uint32_t GetUsableBy() const {
        return _usable_by;
    }

    const std::vector<GlobalSpirit *>& GetSpiritSlots() const {
        return _spirit_slots;
    }

    //! \brief Gives the list of learned skill thanks to this piece of equipment.
    const std::vector<uint32_t>& GetEquipmentSkills() const {
        return _equipment_skills;
    }

private:
    //! \brief The amount of physical defense that the armor provides
    uint32_t _physical_defense;

    //! \brief The amount of magical defense that the armor provides against each elements
    uint32_t _magical_defense;

    /** \brief A bit-mask that determines which characters can use or equip the object
    *** See the game character ID constants in global_actors.h for more information
    **/
    uint32_t _usable_by;

    /** \brief Sockets which may be used to place spirits on the armor
    *** Armor may have no sockets, so it is not uncommon for the size of this vector to be zero.
    *** When a socket is available but empty (has no attached spirit), the pointer at that index
    *** will be nullptr.
    **/
    std::vector<GlobalSpirit *> _spirit_slots;
}; // class GlobalArmor : public GlobalObject

} // namespace vt_global

#endif // __GLOBAL_ARMOR_HEADER__
