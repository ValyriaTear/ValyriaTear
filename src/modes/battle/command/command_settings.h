////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_COMMAND_SETTINGS_HEADER__
#define __BATTLE_COMMAND_SETTINGS_HEADER__

#include "modes/battle/battle_target.h"

#include "common/gui/menu_window.h"
#include "common/gui/option.h"

namespace vt_battle
{

namespace private_battle
{

class BattleCharacter;

//! \brief Action Type Constants
enum COMMAND_CATEGORY {
    CATEGORY_WEAPON    = 0,
    CATEGORY_MAGIC     = 1,
    CATEGORY_SPECIAL   = 2,
    CATEGORY_ITEM      = 3
};

/** ****************************************************************************
*** \brief Manages skill selection lists and cursor memory for an individual character
***
*** Each character participating in the battle will have an instance of this class
*** created for it. The class has two primary functions. First, it creates and
*** manages OptionBox objects for the character's attack, defense, and support skills.
*** Second, it retains the previous selections that were made for that character.
***
*** The class retains three different types of previous targets rather than a single target.
*** These three target members are used to retain different target types (self, ally, and foe).
*** Party targets (all allies, all foes) are not retained since there are not mulitple possibilites
*** for those target types. The self target types also only have a single actor target (the character
*** himself/herself), but it is still used because the character can target multiple points on itself.
*** ***************************************************************************/
class CommandSettings
{
public:
    /** \param character A pointer to the character represented by this class
    *** \param window A reference to the MenuWindow that should be the owner of the GUI displays
    **/
    CommandSettings(BattleCharacter* character, vt_gui::MenuWindow& window);

    ~CommandSettings()
    {}

    /** \brief Refreshes all entries in the attack, defense, and support lists
    *** This should be called whenever the character's current skill points has changed or may have
    *** changed. This method will go through all three skill lists and use the character's current
    *** skill points to determine whether each entry should be enabled or disabled.
    **/
    void RefreshLists();

    /** \brief Sets the appropriate last target member based on the argument target type
    *** \param target A reference to the target to save
    ***
    *** This function will not complain if given any valid target type, including party type targets.
    *** Party targets will simply be ignored since those target types are not retained. Otherwise if
    *** the target type is not invalid, the appropriate last target will be set.
    **/
    void SaveLastTarget(BattleTarget &target);

    //! \name Class member accessor methods
    //@{
    void SetLastCategory(uint32_t category) {
        _last_category = category;
    }

    void SetLastItem(uint32_t item) {
        _last_item = item;
    }

    BattleCharacter *GetCharacter() const {
        return _character;
    }

    uint32_t GetLastCategory() const {
        return _last_category;
    }

    uint32_t GetLastItem() const {
        return _last_item;
    }

    BattleTarget GetLastSelfTarget() const {
        return _last_self_target;
    }

    BattleTarget GetLastCharacterTarget() const {
        return _last_character_target;
    }

    BattleTarget GetLastEnemyTarget() const {
        return _last_enemy_target;
    }

    vt_gui::OptionBox* GetWeaponSkillList() {
        return &_weapon_skill_list;
    }
    vt_gui::OptionBox* GetWeaponTargetList() {
        return &_weapon_target_list;
    }

    vt_gui::OptionBox* GetMagicSkillList() {
        return &_magic_skill_list;
    }
    vt_gui::OptionBox* GetMagicTargetList() {
        return &_magic_target_list;
    }

    vt_gui::OptionBox* GetSpecialSkillList() {
        return &_special_skill_list;
    }
    vt_gui::OptionBox* GetSpecialTargetList() {
        return &_special_target_list;
    }
    //@}

private:
    //! \brief A pointer to the character whose properties are represented by this class
    BattleCharacter* _character;

    //! \brief The last category of action that the player selected for this character
    uint32_t _last_category;

    //! \brief The index of the last item that the player selected for this character
    uint32_t _last_item;

    //! \brief Holds the last attack point that the player selected for the character to target on themselves
    BattleTarget _last_self_target;

    //! \brief The last character target that the player selected for this character
    BattleTarget _last_character_target;

    //! \brief The last enemy target that the player selected for this character
    BattleTarget _last_enemy_target;

    //! \brief A display list of all usable weapon skills
    vt_gui::OptionBox _weapon_skill_list;
    vt_gui::OptionBox _weapon_target_list;

    //! \brief A display list of all usable magic skills
    vt_gui::OptionBox _magic_skill_list;
    vt_gui::OptionBox _magic_target_list;

    //! \brief A display list of all usable items
    vt_gui::OptionBox _special_skill_list;
    vt_gui::OptionBox _special_target_list;
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_COMMAND_SETTINGS_HEADER__
