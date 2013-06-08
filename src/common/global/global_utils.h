////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_utils.h
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for global game utility code
***
*** This file contains several common constants, enums, and functions that are
*** used by various global classes.
*** ***************************************************************************/

#ifndef __GLOBAL_UTILS_HEADER__
#define __GLOBAL_UTILS_HEADER__

#include "utils.h"
#include "engine/video/image.h"

#include <map>

namespace vt_audio {
class SoundDescriptor;
}

namespace vt_video {
class StillImage;
};

namespace vt_global
{

class GlobalObject;

namespace private_global
{

/** \name Object ID Range Constants
*** These constants set the maximum valid ID ranges for each object category.
*** The full valid range for each object category ID is:
*** - Items:            1-10000
*** - Weapons:      10001-20000
*** - Head Armor:   20001-30000
*** - Torso Armor:  30001-40000
*** - Arm Armor:    40001-50000
*** - Leg Armor:    50001-60000
*** - Spirits:      60001-70000
*** - Key Items:    70001-80000 // Old, now any kind of item can be a key item.
**/
//@{
const uint32 OBJECT_ID_INVALID   = 0;
const uint32 MAX_ITEM_ID         = 10000;
const uint32 MAX_WEAPON_ID       = 20000;
const uint32 MAX_HEAD_ARMOR_ID   = 30000;
const uint32 MAX_TORSO_ARMOR_ID  = 40000;
const uint32 MAX_ARM_ARMOR_ID    = 50000;
const uint32 MAX_LEG_ARMOR_ID    = 60000;
const uint32 MAX_SPIRIT_ID       = 70000;
const uint32 MAX_KEY_ITEM_ID     = 80000;
const uint32 OBJECT_ID_EXCEEDS   = 80001;
//@}

/** \name Skill ID Range Constants
*** These constants set the maximum valid ID ranges for each skill category.
*** The full valid range for each skill category ID is:
*** - Weapon:         1-10000
*** - Magic:      10001-20000
*** - Special:    20001-30000
*** - Bare-hands: 30001-40000
**/
//@{
const uint32 MAX_WEAPON_SKILL_ID     = 10000;
const uint32 MAX_MAGIC_SKILL_ID      = 20000;
const uint32 MAX_SPECIAL_SKILL_ID    = 30000;
const uint32 MAX_BARE_HANDS_SKILL_ID = 40000;
//@}

} // namespace private_global

/** \name Target Types
*** \brief Enum values used for declaring the type of targets for items, skills, and actions.
***
*** There are three types of groups within these target types:
*** - Attack Point Types: GLOBAL_TARGET_SELF_POINT, GLOBAL_TARGET_ALLY_POINT, GLOBAL_TARGET_FOE_POINT
*** - Actor Types: GLOBAL_TARGET_SELF, GLOBAL_TARGET_ALLY, GLOBAL_TARGET_FOE
*** - Party Types: GLOBAL_TARGET_ALL_ALLIES, GLOBAL_TARGET_ALL_FOES
***
*** \note The phrasing of "ally" and "foe" in these enum names is relative to the context it is used in.
*** For example, a character using a skill that targets an ally will target one of the other characters in
*** the party while a foe target will select an enemy on the opposing party. On the other hand, if an enemy
*** to the character party is selecting an ally target it will target one of the other enemies in the party
*** and the foe of the enemy is a character.
**/
enum GLOBAL_TARGET {
    GLOBAL_TARGET_INVALID        = -1,
    GLOBAL_TARGET_SELF_POINT     =  0,
    GLOBAL_TARGET_ALLY_POINT     =  1, //!< \note This includes allowing the user to target itself
    GLOBAL_TARGET_FOE_POINT      =  2,
    GLOBAL_TARGET_SELF           =  3,
    GLOBAL_TARGET_ALLY           =  4, //!< \note This includes allowing the user to target itself
    GLOBAL_TARGET_ALLY_EVEN_DEAD =  5,
    GLOBAL_TARGET_FOE            =  6,
    GLOBAL_TARGET_ALL_ALLIES     =  7,
    GLOBAL_TARGET_ALL_FOES       =  8,
    GLOBAL_TARGET_TOTAL          =  9
};

/** \name GlobalItem and GlobalSkill Usage Cases
*** \brief Enum values used for identification of different game object types
**/
enum GLOBAL_USE {
    GLOBAL_USE_INVALID = -1,
    GLOBAL_USE_FIELD   =  0, //!< Usable only in maps and menus
    GLOBAL_USE_BATTLE  =  1, //!< Usable only in battle
    GLOBAL_USE_ALL     =  2, //!< Usable at any time and any location
    GLOBAL_USE_TOTAL   =  3
};

/** \name Game Character IDs
*** \brief Integers that are used for identification of characters
*** These series of constants are used as bit-masks for determining things such as if the character
*** may use a certain item. Only one bit should be set for each character ID.
***
*** \note The IDs for each character are defined in the dat/global.lua file.
**/
//@{
const uint32 GLOBAL_CHARACTER_INVALID     = 0x00000000;
const uint32 GLOBAL_CHARACTER_ALL         = 0xFFFFFFFF;
//@}

/** \name Character Attack Point Positions
*** \brief Integers that represent the index location of the four attack points and armor types for characters
**/
//@{
const uint32 GLOBAL_POSITION_HEAD     = 0;
const uint32 GLOBAL_POSITION_TORSO    = 1;
const uint32 GLOBAL_POSITION_ARMS     = 2;
const uint32 GLOBAL_POSITION_LEGS     = 3;
const uint32 GLOBAL_POSITION_INVALID  = 4;
//@}

//! \brief The maximum number of characters that can be in the active party
const uint32 GLOBAL_MAX_PARTY_SIZE = 4;

/** \name GlobalObject Types
*** \brief Used for identification of different game object types
**/
enum GLOBAL_OBJECT {
    GLOBAL_OBJECT_INVALID     = -1,
    GLOBAL_OBJECT_ITEM        =  0,
    GLOBAL_OBJECT_WEAPON      =  1,
    GLOBAL_OBJECT_HEAD_ARMOR  =  2,
    GLOBAL_OBJECT_TORSO_ARMOR =  3,
    GLOBAL_OBJECT_ARM_ARMOR   =  4,
    GLOBAL_OBJECT_LEG_ARMOR   =  5,
    GLOBAL_OBJECT_SPIRIT      =  6,
    GLOBAL_OBJECT_TOTAL       =  7
};

//! \brief translate the object type into the object position.
uint32 GetEquipmentPositionFromObjectType(GLOBAL_OBJECT object_type);

/** \name Elemental Effect Types
*** \brief Used to identify the eight different types of elementals
*** There are a total of four physical and four magical elemental effects
**/
enum GLOBAL_ELEMENTAL {
    GLOBAL_ELEMENTAL_INVALID    = -1,
    GLOBAL_ELEMENTAL_FIRE       =  0,
    GLOBAL_ELEMENTAL_WATER      =  1,
    GLOBAL_ELEMENTAL_VOLT       =  2,
    GLOBAL_ELEMENTAL_EARTH      =  3,
    GLOBAL_ELEMENTAL_LIFE       =  4,
    GLOBAL_ELEMENTAL_DEATH      =  5,
    GLOBAL_ELEMENTAL_NEUTRAL    =  6,
    GLOBAL_ELEMENTAL_TOTAL      =  7
};

/** \name Status Effect Types
*** \brief Used to identify the various types of status effects
**/
enum GLOBAL_STATUS {
    GLOBAL_STATUS_INVALID           = -1,
    GLOBAL_STATUS_STRENGTH_RAISE    =  0,
    GLOBAL_STATUS_STRENGTH_LOWER    =  1,
    GLOBAL_STATUS_VIGOR_RAISE       =  2,
    GLOBAL_STATUS_VIGOR_LOWER       =  3,
    GLOBAL_STATUS_FORTITUDE_RAISE   =  4,
    GLOBAL_STATUS_FORTITUDE_LOWER   =  5,
    GLOBAL_STATUS_PROTECTION_RAISE  =  6,
    GLOBAL_STATUS_PROTECTION_LOWER  =  7,
    GLOBAL_STATUS_AGILITY_RAISE     =  8,
    GLOBAL_STATUS_AGILITY_LOWER     =  9,
    GLOBAL_STATUS_EVADE_RAISE       = 10,
    GLOBAL_STATUS_EVADE_LOWER       = 11,
    GLOBAL_STATUS_HP_REGEN          = 12,
    GLOBAL_STATUS_HP_DRAIN          = 13,
    GLOBAL_STATUS_SP_REGEN          = 14,
    GLOBAL_STATUS_SP_DRAIN          = 15,
    GLOBAL_STATUS_PARALYSIS         = 16,
    GLOBAL_STATUS_FIRE_RAISE        = 17,
    GLOBAL_STATUS_FIRE_LOWER        = 18,
    GLOBAL_STATUS_WATER_RAISE       = 19,
    GLOBAL_STATUS_WATER_LOWER       = 20,
    GLOBAL_STATUS_VOLT_RAISE        = 21,
    GLOBAL_STATUS_VOLT_LOWER        = 22,
    GLOBAL_STATUS_EARTH_RAISE       = 23,
    GLOBAL_STATUS_EARTH_LOWER       = 24,
    GLOBAL_STATUS_LIFE_RAISE        = 25,
    GLOBAL_STATUS_LIFE_LOWER        = 26,
    GLOBAL_STATUS_DEATH_RAISE       = 27,
    GLOBAL_STATUS_DEATH_LOWER       = 28,
    GLOBAL_STATUS_NEUTRAL_RAISE     = 29,
    GLOBAL_STATUS_NEUTRAL_LOWER     = 30,
    GLOBAL_STATUS_TOTAL             = 31
};

/** \name Effect Intensity Levels
*** \brief Used to reflect the potency of elemental and status effects
*** There are nine valid intensity levels. Four negative, four positive, and one neutral.
*** The neutral intensity level essentially equates to "no effect".
**/
enum GLOBAL_INTENSITY {
    GLOBAL_INTENSITY_INVALID       = -5,
    GLOBAL_INTENSITY_NEG_EXTREME   = -4,
    GLOBAL_INTENSITY_NEG_GREATER   = -3,
    GLOBAL_INTENSITY_NEG_MODERATE  = -2,
    GLOBAL_INTENSITY_NEG_LESSER    = -1,
    GLOBAL_INTENSITY_NEUTRAL       =  0,
    GLOBAL_INTENSITY_POS_LESSER    =  1,
    GLOBAL_INTENSITY_POS_MODERATE  =  2,
    GLOBAL_INTENSITY_POS_GREATER   =  3,
    GLOBAL_INTENSITY_POS_EXTREME   =  4,
    GLOBAL_INTENSITY_TOTAL         =  5
};

/** \name Skill Types
*** \brief Enum values used to identify the type of a skill.
**/
enum GLOBAL_SKILL {
    GLOBAL_SKILL_INVALID     = -1,
    GLOBAL_SKILL_WEAPON      =  0,
    GLOBAL_SKILL_MAGIC       =  1,
    GLOBAL_SKILL_SPECIAL     =  2,
    GLOBAL_SKILL_BARE_HANDS  =  3,
    GLOBAL_SKILL_TOTAL       =  4
};

//! \brief The Battle enemies harm levels
enum GLOBAL_ENEMY_HURT {
    GLOBAL_ENEMY_HURT_NONE     = 0,
    GLOBAL_ENEMY_HURT_SLIGHTLY = 1,
    GLOBAL_ENEMY_HURT_MEDIUM   = 2,
    GLOBAL_ENEMY_HURT_HEAVILY  = 3,
    GLOBAL_ENEMY_HURT_TOTAL    = 4,
};

/** \brief Retrieves a string representation for any GLOBAL_TARGET enum value
*** \param target The target enum value to find the string for
*** \return Translated text that describes the target
**/
std::string GetTargetText(GLOBAL_TARGET target);

/** \brief Returns true if the target selects an attack point
*** \param target The target enum value to analyze
**/
bool IsTargetPoint(GLOBAL_TARGET target);

/** \brief Returns true if the target selects an actor
*** \param target The target enum value to analyze
**/
bool IsTargetActor(GLOBAL_TARGET target);

/** \brief Returns true if the target selects a party
*** \param target The target enum value to analyze
**/
bool IsTargetParty(GLOBAL_TARGET target);

/** \brief Returns true if the target selects the user
*** \param target The target enum value to analyze
**/
bool IsTargetSelf(GLOBAL_TARGET target);

/** \brief Returns true if the target selects an ally
*** \param target The target enum value to analyze
**/
bool IsTargetAlly(GLOBAL_TARGET target);

/** \brief Returns true if the target selects a foe
*** \param target The target enum value to analyze
**/
bool IsTargetFoe(GLOBAL_TARGET target);

/** \brief Creates a new GlobalObject and returns a pointer to it
*** \param id The id value of the object to create
*** \param count The count of the new object to create (default value == 1)
*** \return A pointer to the newly created GlobalObject, or NULL if the object could not be created
***
*** This function does not actually create a GlobalObject (it can't since its an abstract class).
*** It creates one of the derived object class types depending on the value of the id argument.
**/
GlobalObject *GlobalCreateNewObject(uint32 id, uint32 count = 1);

/** \brief Increments a GLOBAL_INTENSITY enumerated value
*** \param intensity A reference to the intensity data to modify
*** \param amount The number of levels to increase the intensity by (default == 1)
*** \return True if the intensity data was modified or false if it was left unchanged
*** \note The intensity will not be allowed to increase beyond the valid intensity range
**/
bool IncrementIntensity(GLOBAL_INTENSITY &intensity, uint8 amount = 1);

/** \brief Decrements a GLOBAL_INTENSITY enumerated value
*** \param intensity A reference to the intensity data to modify
*** \param amount The number of levels to decrease the intensity by (default == 1)
*** \return True if the intensity data was modified or false if it was left unchanged
*** \note The intensity will not be allowed to decrease beyond the valid intensity range
**/
bool DecrementIntensity(GLOBAL_INTENSITY &intensity, uint8 amount = 1);

//! Gives the opposite status effect if there is one of GLOBAL_STATUS_INVALID if none.
GLOBAL_STATUS GetOppositeStatusEffect(GLOBAL_STATUS status_effect);

//! Gives the opposite effect intensity if there is one of GLOBAL_INTENSITY_INVALID if none.
GLOBAL_INTENSITY GetOppositeIntensity(GLOBAL_INTENSITY intensity);

/** \brief A simple class used to store commonly used media files.
*** It is used as a member of the game global class.
**/
class GlobalMedia {
public:
    GlobalMedia() {}

    ~GlobalMedia();

    //! \brief Loads all the media files.
    //! Should be called after the final intialization of the VideoManager as
    //! the texture manager is ready only afterward.
    void Initialize();

    vt_video::StillImage* GetDrunesIcon() {
        return &_drunes_icon;
    }

    vt_video::StillImage* GetStarIcon() {
        return &_star_icon;
    }

    vt_video::StillImage* GetCheckIcon() {
        return &_check_icon;
    }

    vt_video::StillImage* GetXIcon() {
        return &_x_icon;
    }

    vt_video::StillImage* GetSpiritSlotIcon() {
        return &_spirit_slot_icon;
    }

    vt_video::StillImage* GetEquipIcon() {
        return &_equip_icon;
    }

    vt_video::StillImage* GetKeyItemIcon() {
        return &_key_item_icon;
    }

    vt_video::StillImage* GetClockIcon() {
        return &_clock_icon;
    }

    vt_video::StillImage* GetBottomMenuImage() {
        return &_bottom_menu_image;
    }

    std::vector<vt_video::StillImage>* GetAllItemCategoryIcons() {
        return &_all_category_icons;
    }

    /** \brief Retrieves the category icon image that represents the specified object type
    *** \param object_type The type of the global object to retrieve the icon for
    *** \return A pointer to the image holding the category's icon. NULL if the argument was invalid.
    *** \note GLOBAL_OBJECT_TOTAL will return the icon for "all wares"
    **/
    vt_video::StillImage* GetItemCategoryIcon(GLOBAL_OBJECT object_type);

    /** \brief Retrieves a specific elemental icon with the proper type and intensity
    *** \param element_type The type of element the user is trying to retrieve the icon for
    *** \param intensity The intensity level of the icon to retrieve
    *** \return The icon representation of the element type and intensity
    **/
    vt_video::StillImage* GetElementalIcon(GLOBAL_ELEMENTAL element_type, GLOBAL_INTENSITY intensity);

    /** \brief Retrieves a specific status icon with the proper type and intensity
    *** \param status_type The type of status the user is trying to retrieve the icon for
    *** \param intensity The intensity level of the icon to retrieve
    *** \return The icon representation of the status type and intensity
    **/
    vt_video::StillImage* GetStatusIcon(GLOBAL_STATUS status_type, GLOBAL_INTENSITY intensity);

    /** \brief Plays a sound object previoulsy loaded
    *** \param identifier The string identifier for the sound to play
    **/
    void PlaySound(const std::string &identifier);

private:
    //! \brief Retains icon images for all possible object categories, including "all wares"
    std::vector<vt_video::StillImage> _all_category_icons;

    //! \brief Image icon representing drunes (currency)
    vt_video::StillImage _drunes_icon;

    //! \brief Image icon of a single yellow/gold star
    vt_video::StillImage _star_icon;

    //! \brief Image icon of a green check mark
    vt_video::StillImage _check_icon;

    //! \brief Image icon of a red x
    vt_video::StillImage _x_icon;

    //! \brief Image icon representing open spirit slots available on weapons and armors
    vt_video::StillImage _spirit_slot_icon;

    //! \brief Image icon that represents when a character has a weapon or armor equipped
    vt_video::StillImage _equip_icon;

    //! \brief The Key item icon
    vt_video::StillImage _key_item_icon;

    //! \brief The clock icon
    vt_video::StillImage _clock_icon;

    //! \brief Retains all icon images that represent the game's elementals
    std::vector<vt_video::StillImage> _elemental_icons;

    //! \brief Retains all icon images that represent the game's status effects
    std::vector<vt_video::StillImage> _status_icons;

    //! \brief The battle and boot bottom image
    vt_video::StillImage _bottom_menu_image;

    //! \brief A map of the sounds used in different game modes
    std::map<std::string, vt_audio::SoundDescriptor*> _sounds;

    //! \brief Loads a sound file and add it to the sound map
    void _LoadSoundFile(const std::string& sound_name, const std::string& filename);
};

} // namespace vt_global

#endif // __GLOBAL_UTILS_HEADER__
