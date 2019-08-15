////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ITEM_HEADER__
#define __GLOBAL_ITEM_HEADER__

#include "global_object.h"
#include "common/global/global_target.h"

#include "script/script_read.h"

namespace vt_global
{

//! \brief The different item categories
enum ITEM_CATEGORY {
    ITEM_ALL = 0,
    ITEM_ITEM = 1,
    ITEM_WEAPON = 2,
    ITEM_HEAD_ARMOR = 3,
    ITEM_TORSO_ARMOR = 4,
    ITEM_ARMS_ARMOR = 5,
    ITEM_LEGS_ARMOR = 6,
    ITEM_KEY = 7,
    ITEM_CATEGORY_SIZE = 8
};

/** ****************************************************************************
*** \brief Represents items used throughout the game
***
*** This class is for general use items such as healing potions. Each item has a
*** different effect when it is used, implemented by a Lua function written
*** specifically for the item which calls it. Some items may be used only in certain
*** scenarios (in battles, on the field, etc.). All items may be used by any
*** character or enemy in the game.
*** ***************************************************************************/
class GlobalItem : public GlobalObject
{
public:
    /** \param id The unique ID number of the item
    *** \param count The number of items to initialize this class object as representing (default value == 1)
    **/
    explicit GlobalItem(uint32_t id, uint32_t count = 1);
    virtual ~GlobalItem() override
    {
    }

    GlobalItem(const GlobalItem& copy);
    GlobalItem& operator=(const GlobalItem& copy);

    GLOBAL_OBJECT GetObjectType() const override {
        return GLOBAL_OBJECT_ITEM;
    }

    //! \brief Returns true if the item can be used in battle
    bool IsUsableInBattle() const {
        return _battle_use_function.is_valid();
    }

    //! \brief Returns true if the item can be used in the field
    bool IsUsableInField() const {
        return _field_use_function.is_valid();
    }

    //! \name Class Member Access Functions
    //@{
    GLOBAL_TARGET GetTargetType() const {
        return _target_type;
    }

    //! \brief Returns the Battle warmup script function reference
    const luabind::object& GetBattleWarmupFunction() const {
        return _battle_warmup_function;
    }

    /** \brief Returns a pointer to the luabind::object of the battle use function
    *** \note This function will return nullptr if the skill is not usable in battle
    **/
    const luabind::object& GetBattleUseFunction() const {
        return _battle_use_function;
    }

    /** \brief Returns a pointer to the luabind::object of the field use function
    *** \note This function will return nullptr if the skill is not usable in the field
    **/
    const luabind::object& GetFieldUseFunction() const {
        return _field_use_function;
    }

    //! \brief Returns Warmup time needed before using this item in battles.
    inline uint32_t GetWarmUpTime() const {
        return _warmup_time;
    }

    //! \brief Returns Warmup time needed before using this item in battles.
    inline uint32_t GetCoolDownTime() const {
        return _cooldown_time;
    }

    /** \brief Tells the animation script filename linked to the skill for the given character,
    *** Or an empty value otherwise;
    **/
    std::string GetAnimationScript(uint32_t character_id) const;
    //@}

private:
    //! \brief The type of target for the item
    GLOBAL_TARGET _target_type;

    //! \brief A reference to the script performing the warmup action during battle
    luabind::object _battle_warmup_function;

    //! \brief A reference to the script function that performs the item's effect while in battle
    luabind::object _battle_use_function;

    //! \brief A reference to the script function that performs the item's effect while in a menu
    luabind::object _field_use_function;

    //! \brief The warmup time in milliseconds needed before using this item in battles.
    uint32_t _warmup_time;

    //! \brief The cooldown time in milliseconds needed after using this item in battles.
    uint32_t _cooldown_time;

    //! \brief map containing the animation scripts names linked to each characters id for the given skill.
    std::map <uint32_t, std::string> _animation_scripts;
};

} // namespace vt_global

#endif // __GLOBAL_ITEM_HEADER__
