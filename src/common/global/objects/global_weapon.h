////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_WEAPON_HEADER__
#define __GLOBAL_WEAPON_HEADER__

#include "global_object.h"

namespace vt_global
{

class GlobalSpirit;

/** ****************************************************************************
*** \brief Represents weapon that may be equipped by characters or enemies
***
*** All classes of weapons (swords, bows, spears, etc.) are represented by this
*** class. Typically, a weapon may only be used by a select few and can not be
*** equipped on every character. Weapons have two attack ratings: physical
*** and magical, both of which are included in the damage calculation
*** formulae when a character or enemy attacks using the weapon. Weapons may also
*** have a small number of slots in which spirits can be merged to improve
*** or alter the weapon's properties. Some weapons have zero sockets available.
*** Finally, weapons may come imbued with certain elemental or status effect
*** properties that are inflicted on a target.
*** ***************************************************************************/
class GlobalWeapon : public GlobalObject
{
public:
    /** \param id The unique ID number of the weapon
    *** \param count The number of weapons to initialize this class object as representing (default value == 1)
    **/
    explicit GlobalWeapon(uint32_t id, uint32_t count = 1);
    virtual ~GlobalWeapon() override
    {
    }

    GLOBAL_OBJECT GetObjectType() const override {
        return GLOBAL_OBJECT_WEAPON;
    }

    //! \name Class Member Access Functions
    //@{
    uint32_t GetPhysicalAttack() const {
        return _physical_attack;
    }

    uint32_t GetMagicalAttack() const {
        return _magical_attack;
    }

    uint32_t GetUsableBy() const {
        return _usable_by;
    }

    const std::vector<GlobalSpirit *>& GetSpiritSlots() const {
        return _spirit_slots;
    }

    const std::string& GetAmmoAnimationFile() const {
        return _ammo_animation_file;
    }

    //! \brief Get the animation filename corresponding to the character weapon animation
    //! requested.
    const std::string& GetWeaponAnimationFile(uint32_t character_id, const std::string& animation_alias);

    //! \brief Gives the list of learned skill thanks to this piece of equipment.
    const std::vector<uint32_t>& GetEquipmentSkills() const {
        return _equipment_skills;
    }
    //@}

private:
    //! \brief The battle image animation file used to display the weapon ammo.
    std::string _ammo_animation_file;

    //! \brief The amount of physical damage that the weapon causes
    uint32_t _physical_attack;

    //! \brief The amount of magical damage that the weapon causes for each elements.
    uint32_t _magical_attack;

    /** \brief A bit-mask that determines which characters can use or equip the object
    *** See the game character ID constants in global_actors.h for more information
    **/
    uint32_t _usable_by;

    //! \brief The info about weapon animations for each global character.
    //! map < character_id, map < animation alias, animation filename > >
    std::map <uint32_t, std::map<std::string, std::string> > _weapon_animations;

    /** \brief Spirit slots which may be used to place spirits on the weapon
    *** Weapons may have no slots, so it is not uncommon for the size of this vector to be zero.
    *** When spirit slots are available but empty (has no attached spirit), the pointer at that index
    *** will be nullptr.
    **/
    std::vector<GlobalSpirit *> _spirit_slots;

    //! \brief Loads the battle animations data for each character that can use the weapon.
    void _LoadWeaponBattleAnimations(vt_script::ReadScriptDescriptor& script);
}; // class GlobalWeapon : public GlobalObject

} // namespace vt_global

#endif // __GLOBAL_WEAPON_HEADER__
