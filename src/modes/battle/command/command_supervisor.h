////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLE_COMMAND_SUPERVISOR_HEADER__
#define __BATTLE_COMMAND_SUPERVISOR_HEADER__

#include "command_settings.h"

#include "modes/battle/battle_item.h"

#include "item_command.h"
#include "skill_command.h"

namespace vt_battle
{

namespace private_battle
{

class BattleCharacter;

//! \brief Enums for the various states that the CommandSupervisor class may be in
enum COMMAND_STATE {
    COMMAND_STATE_INVALID         = -1,
    //! Player is selecting the type of action to execute
    COMMAND_STATE_CATEGORY        = 0,
    //! Player is selecting from a list of actions to execute
    COMMAND_STATE_ACTION          = 1,
    //! Player is selecting the actor target to execute the action on
    COMMAND_STATE_ACTOR           = 2,
    //! Player is selecting the point target to execute the action on
    COMMAND_STATE_POINT           = 3,
    COMMAND_STATE_TOTAL           = 4
};

/** ****************************************************************************
*** \brief Manages input and display to allow the player to select commands for their characters
***
*** This class is somewhat of an extension to the BattleMode class. It takes effect whenever a
*** character in the battle enters the command state, which in turn causes BattleMode to enter the
*** command state and initialize this class. The contents of the command supervisor are drawn to a
*** medium-size window in the lower right corner of the screen. The standard flow of this class
*** proceeds as follows.
***
*** -# Class is initialized with a character that has entered the command state
*** -# The player selects an action category for the character
*** -# The player selects an action for the character, which may be to execute a skill or use an item
*** -# The player selects a target for the action, finalizing the command
***
*** The player may backtrack through this flow of events and change their previous selections.
***
*** This class also enables cursor memory on a per-character basis. It does this by retaining an instance
*** of CharacterCommandSettings for each character. These objects retain all the previous selections
*** for the characters, as well as containing the option boxes for the attack, defense, and support
*** skills of the characters.
***
*** The ItemCommand and SkillCommand classes are assistants to this class, and this class creates an
*** instance of each type. Sometimes this class will receive notification messages to indicate when
*** certain events occur while the command supervisor is active. For example, if the character that
*** the player is selecting a command for is killed, this class will have to abort its operation.
*** Another example would be when another character has not used an item that they had intended to
*** (either due to that character's death or being unable to find a valid target). In this situation,
*** the item list needs to be updated to reflect the newly available item. Notifications are sent to
*** this class by BattleMode, and it may pass on some notifacations to the ItemCommand and SkillCommand
*** class where appropriate.
*** ***************************************************************************/
class CommandSupervisor
{
public:
    CommandSupervisor();

    ~CommandSupervisor();

    /** \brief Builds all of the varous command menus and prepares them for use
    *** This should only be invoked once after the BattleMode class has initialized all of its
    *** character actors. It will create a command menu for the list of usable items as well as
    *** the various skill menus for each character in the party.
    **/
    void ConstructMenus();

    /** \brief Resets the command supervisor state and members
    *** \param character A pointer to the character that the player should select a command for
    **/
    void Initialize(BattleCharacter *character);

    //! \brief Returns a pointer to the character that the player is selecting a command for
    BattleCharacter *GetCommandCharacter() const {
        if(_active_settings == nullptr) return nullptr;
        else return _active_settings->GetCharacter();
    }

    //! \brief Updates the state of the command selection
    void Update();

    //! \brief Draws the command window and contents to the screen
    void Draw();

    /** \brief Called whenever an actor dies while the command supervisor is active
    *** \param actor A pointer to the actor who is now deceased
    ***
    *** If the actor who died is the active character that the player is selecting an action for, this
    *** will cause the supervisor to return to the invalid state. If the actor who died is the selected
    *** target, the next available valid target will be selected instead.
    **/
    void NotifyActorDeath(private_battle::BattleActor *actor);

    /** \brief Cancels the current command
    ***
    *** This function cancels the command without checking whether the battle 
    *** is in WAIT, SEMI-ACTIVE or ACTIVE mode. If this check is desired, it 
    *** must be done by the caller.
    **/
    void CancelCurrentCommand();

    //! \name Class member accessor methods
    //@{
    COMMAND_STATE GetState() const {
        return _state;
    }

    BattleTarget GetSelectedTarget() const {
        return _selected_target;
    }

    /** \brief Apply battle items count on inventory items.
    *** This will have to be called upon victory.
    **/
    void CommitChangesToInventory() {
        _item_command.CommitChangesToInventory();
    }

    //! \brief Reset the item list content, used at battle restart
    void ResetItemList() {
        _item_command.ResetItemList();
    }
    //@}

private:
    //! \brief The state that the action window is in, which reflects the contents of the window
    COMMAND_STATE _state;

    //! \brief A pointer to the settings for the active character
    CommandSettings* _active_settings;

    //! \brief A pointer to the skill that is currently selected, if any
    vt_global::GlobalSkill* _selected_skill;

    //! \brief A pointer to the item that is currently selected, if any
    std::shared_ptr<BattleItem> _selected_item;

    //! \brief Retains the target that the player has selected
    BattleTarget _selected_target;

    //! \brief An instance for managing the character party's inventory
    ItemCommand _item_command;

    //! \brief An instance for managing the skills of the active character
    SkillCommand _skill_command;

    //! \brief A container of setting objects for each character in the battle
    std::map<BattleCharacter *, CommandSettings> _character_settings;

    // ---------- Graphics and GUI members

    //! \brief Contains the icon images that represent each action category
    std::vector<vt_video::StillImage> _category_icons;

    //! \brief Contains the text that represent each action category
    std::vector<vt_video::TextImage> _category_text;

    //! \brief The window where all actions are drawn
    vt_gui::MenuWindow _command_window;

    //! \brief Header text
    vt_video::TextImage _window_header;

    //! \brief Rendered text that contains information about the currently selected target
    vt_video::TextImage _selected_target_name;

    //! \brief The current actor status effects.
    std::vector<vt_video::StillImage*> _selected_target_status_effects;

    //! \brief The status effects applied with a chance when aiming a body part.
    std::vector<vt_video::StillImage*> _selected_attack_point_status_effects;

    //! \brief The window where all information about the currently selected action is drawn
    vt_gui::MenuWindow _info_window;

    //! \brief Info text header
    vt_video::TextImage _info_header;

    //! \brief Info text text
    vt_video::TextImage _info_text;

    /** \brief The option box that lists the types of actions that a character may take in battle
    *** Typically this list includes "attack", "defend", "support", and "item". More types may appear
    *** under special circumstances and conditions.
    **/
    vt_gui::OptionBox _category_options;

    /** \brief Contains a list of the possible targets that a player may select from
    ***
    *** This option box is used for the selection of both actors and attack points.
    **/
    vt_gui::OptionBox _target_options;

    //! \brief Stores whether the information window should be shown
    bool _show_information;

    // ---------- Private methods

    //! \brief Returns true if the selected action category is a skill action
    bool _IsSkillCategorySelected() const;

    //! \brief Returns true if the selected action category is an item action
    bool _IsItemCategorySelected() const;

    //! \brief Returns the type of target for the selected action
    vt_global::GLOBAL_TARGET _ActionTargetType();

    //! \brief Returns true if the character parameter has already had a settings instance created for it
    bool _HasCharacterSettings(BattleCharacter *character) const {
        if(character == nullptr) return false;
        else return (_character_settings.find(character) != _character_settings.end());
    }

    /** \brief Creates and stores a CharacterCommandSettings instance for a given character
    *** \param character A pointer to the character to create the command settings for
    *** \note This function will not check whether or not an instance of CharacterCommandSettings has
    *** already been created for this character. That check should be done externally to this function
    *** call.
    **/
    void _CreateCharacterSettings(BattleCharacter *character) {
        _character_settings.insert(std::make_pair(character, CommandSettings(character, _command_window)));
    }

    //! \brief Initializes the _selected_target member with an initial target for the currently selected action
    bool _SetInitialTarget();

    /** \brief Changes the active command state and performs any necessary state transactions
    *** \param new_state The state to change to
    **/
    void _ChangeState(COMMAND_STATE new_state);

    //! \brief Updates state when the player is selecting an action category
    void _UpdateCategory();

    //! \brief Updates state when the player is selecting an action
    void _UpdateAction();

    //! \brief Updates state when the player is selecting an actor target
    void _UpdateActorTarget();

    //! \brief Updates state when the player is selecting an attack point target
    void _UpdateAttackPointTarget();

    //! \brief Updates current skill/item information data
    void _UpdateActionInformation();

    //! \brief Draws visible contents to the screen when the player is selecting an action category
    void _DrawCategory();

    //! \brief Draws visible contents to the screen when the player is selecting an action
    void _DrawAction();

    //! \brief Draws visible contents to the screen when the player is selecting an actor target
    void _DrawActorTarget();

    //! \brief Draws visible contents to the screen when the player is selecting an attack point target
    void _DrawAttackPointTarget();

    //! \brief Draws visible contents to the screen when the player is viewing information about an action
    void _DrawActionInformation();

    //! \brief Updates the text for _window_header and _selected_target_name to represent information about the selected target
    void _UpdateActorTargetText();

    //! \brief Sets the text for _window_header and _target_options to represent information about the selected target.
    //! Should be called only when switching to displaying attack points.
    void _CreateAttackPointTargetText();

    //! \brief Finalizes the player's command of the character by creating the appropriate action
    void _FinalizeCommand();
};

} // namespace private_battle

} // namespace vt_battle

#endif // __BATTLE_COMMAND_SUPERVISOR_HEADER__
