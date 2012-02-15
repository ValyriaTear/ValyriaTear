////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_command.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for battle command menu
***
*** This code is responsible for processing input from the player when they are
*** selected an action to execute for one of their characters. This includes
*** displaying the list of actions that the player may take, the target indicator
*** graphics, etc.
*** ***************************************************************************/

#ifndef __BATTLE_COMMAND_HEADER__
#define __BATTLE_COMMAND_HEADER__

#include "utils.h"
#include "defs.h"

#include "gui.h"
#include "global.h"

#include "battle_utils.h"

namespace hoa_battle {

namespace private_battle {

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
class CharacterCommandSettings {
public:
	/** \param character A pointer to the character represented by this class
	*** \param window A reference to the MenuWindow that should be the owner of the GUI displays
	**/
	CharacterCommandSettings(BattleCharacter* character, hoa_gui::MenuWindow& window);

	~CharacterCommandSettings()
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
	void SaveLastTarget(BattleTarget& target);

	//! \name Class member accessor methods
	//@{
	void SetLastCategory(uint32 category)
		{ _last_category = category; }

	void SetLastItem(uint32 item)
		{ _last_item = item; }

	//! \note Only valid types for target parameter are GLOBAL_TARGET_SELF_POINT and GLOBAL_TARGET_SELF
	void SetLastSelfTarget(BattleTarget& target);

	//! \note Only valid types for target parameter are GLOBAL_TARGET_ALLY_POINT and GLOBAL_TARGET_ALLY
	void SetLastCharacterTarget(BattleTarget& target);

	//! \note Only valid types for target parameter are GLOBAL_TARGET_FOE_POINT and GLOBAL_TARGET_FOE
	void SetLastEnemyTarget(BattleTarget& target);

	BattleCharacter* GetCharacter() const
		{ return _character; }

	uint32 GetLastCategory() const
		{ return _last_category; }

	uint32 GetLastItem() const
		{ return _last_item; }

	BattleTarget GetLastSelfTarget() const
		{ return _last_self_target; }

	BattleTarget GetLastCharacterTarget() const
		{ return _last_character_target; }

	BattleTarget GetLastEnemyTarget() const
		{ return _last_enemy_target; }

	hoa_gui::OptionBox* GetAttackList()
		{ return &_attack_list; }

	hoa_gui::OptionBox* GetDefendList()
		{ return &_defend_list; }

	hoa_gui::OptionBox* GetSupportList()
		{ return &_support_list; }
	//@}

private:
	//! \brief A pointer to the character whose properties are represented by this class
	BattleCharacter* _character;

	//! \brief The last category of action that the player selected for this character
	uint32 _last_category;

	//! \brief The index of the last item that the player selected for this character
	uint32 _last_item;

	//! \brief Holds the last attack point that the player selected for the character to target on themselves
	BattleTarget _last_self_target;

	//! \brief The last character target that the player selected for this character
	BattleTarget _last_character_target;

	//! \brief The last enemy target that the player selected for this character
	BattleTarget _last_enemy_target;

	//! \brief A display list of all usable items
	hoa_gui::OptionBox _attack_list;

	//! \brief A display list of all usable items
	hoa_gui::OptionBox _defend_list;

	//! \brief A display list of all usable items
	hoa_gui::OptionBox _support_list;
}; // class CharacterCommandSettings


/** ****************************************************************************
*** \brief Manages the character party's usable items
***
*** This class is an assistant to the CommandSupervisor class. It serves to manage
*** the items that the player may select to use in battle and provides displays
*** to those items. Upon initialization, this class creats a copy of every item in the
*** character party's inventory that may be used in battle. The class also maintains
*** certain GUI displays to assist the CommandSupervisor in displaying the list of
*** items available to use.
***
*** \note In the future we may wish to support the case where a new item that is not
*** currently in the player's inventory is added in the middle of the battle. Support
*** for such a feature would have to be added to and tested in this class first.
*** ***************************************************************************/
class ItemCommand {
public:
	//! \param window A reference to the MenuWindow that the GUI objects should be owned by
	ItemCommand(hoa_gui::MenuWindow& window);

	~ItemCommand()
		{}

	/** \brief Constructs the _item_list option box from scratch using the _items container
	*** This will also reset the selection on the item list to the first element. Typically this only needs to be
	*** called once, during battle mode initialization.
	**/
	void ConstructList();

	/** \brief Initializes the item list by setting the selected list option
	*** \param item_index The index of the item to select
	*** \note If the selection argument is out-of-range, no change will take place.
	**/
	void Initialize(uint32 item_index);

	/** \brief Returns a pointer to the currently selected item
	*** This function will return NULL if the class has not been initialized and there is no list of
	*** items to select from.
	**/
	BattleItem* GetSelectedItem();

	/** \brief Returns the index of the item currently selected in the item list
	*** If the selection is invalid (because the list is empty) or the item could not be found,
	*** the value 0xFFFFFFFF will be returned.
	**/
	uint32 GetItemIndex() const;

	/** \brief Returns a pointer to the item at the selected index
	*** \param item_index The index of the item to retrieve
	*** \return A pointer to the item, or NULL if the item_index argument was out-of-range
	**/
	BattleItem* GetItem(uint32 item_index)
		{ if (item_index >= _items.size()) return NULL; else return &(_items[item_index]); }

	//! \brief Updates the item list and processes user input
	void UpdateList();

	//! \brief Updates the item information and processses user input
	void UpdateInformation();

	//! \brief Draws the item header and list
	void DrawList();

	//! \brief Draws information about the selected item
	void DrawInformation();

	/** \brief Modifies the character party's global inventory to match the counts of the items in this class
	*** This should be called only after the battle has finished. There is no need to modify the party's
	*** inventory while the battle is still progressing.
	**/
	void CommitInventoryChanges();

	//! \brief Retuns the number of items that will be displayed in the list
	uint32 GetNumberListOptions() const
		{ return _item_list.GetNumberOptions(); }

private:
	/** \brief Container for all available items
	*** The order of the items in this container is the same as the order that the items would appear in
	*** the inventory list of menu mode. The size of the container does not change, even when the available
	*** count of a specific item becomes zero.
	**/
	std::vector<BattleItem> _items;

	/** \brief A mapping of each element in the _items vector to its position in the item list
	*** This container is necessary because the _item_list option box does not always have the same number
	*** of options as there are elements in the _items vector, since some items may be used up during the
	*** battle. This container is the same size as the _items vector and the elements it stores are a
	*** mapping to the index representing each item in the _item_list. If _item_list does not contain an
	*** entry for a particular item, a negative value will be stored for that item in this container.
	**/
	std::vector<int32> _item_mappings;

	//! \brief A single line of header text for the item list option box
	hoa_gui::OptionBox _item_header;

	//! \brief A display list of all usable items
	hoa_gui::OptionBox _item_list;

	/** \brief Refreshes a single entry in the _item_list
	*** \param entry An index to the element of the OptionBox to refresh
	***
	*** This method is called whenever the available count for a given item is changed. However if
	*** the count is changing between a zero and non-zero value, this method is not used because the
	*** entire list needs to be reconstructed in this case.
	**/
	void _RefreshEntry(uint32 entry);
}; // class ItemCommand


/** ****************************************************************************
*** \brief Manages the display of skills and skill information
***
*** Although this class manages lists of skills, it does not contain those skills
*** directly. They are contained externally on a per-character basis by the
*** CharacterCommandSettings class.
*** ***************************************************************************/
class SkillCommand {
public:
	//! \param window A reference to the MenuWindow that the GUI objects should be owned by
	SkillCommand(hoa_gui::MenuWindow& window);

	~SkillCommand()
		{}

	/** \brief Initializes the class
	*** \param skills A pointer to the set of skills that the class will
	*** \param skill_list A pointer to the option box
	**/
	void Initialize(std::vector<hoa_global::GlobalSkill*>* skills, hoa_gui::OptionBox* skill_list);

	/** \brief Returns the currently selected skill
	*** This function will return NULL if the class has not been initialized and there is no list of
	*** skills to select from.
	**/
	hoa_global::GlobalSkill* GetSelectedSkill() const;

	/** \brief Returns if the selected skill is enabled
	*** This function will return true, if the selected skill is enabled and false otherwise. It will
	*** also return false, if the class has not been initialized and there is no list of
	*** skills to select from.
	**/
	bool GetSelectedSkillEnabled();

	//! \brief Updates the skill list and processes user input
	void UpdateList();

	//! \brief Updates the skill information and processses user input
	void UpdateInformation();

	//! \brief Draws the skill header and list
	void DrawList();

	//! \brief Draws information about the selected skill
	void DrawInformation();

private:
	//! \brief A pointer to the vector of skills corresponding to the options in _skill_list
	std::vector<hoa_global::GlobalSkill*>* _skills;

	//! \brief A single line of header text for the skill list option box
	hoa_gui::OptionBox _skill_header;

	//! \brief A pointer to the list of skills that the class should operate on
	hoa_gui::OptionBox* _skill_list;
}; // class SkillCommand


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
*** The player may backtrack through this flow of events and change their previous selections. There
*** also exists an optional state where the player can view information about a selected skill or item
*** during the action selection process.
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
class CommandSupervisor {
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
	void Initialize(BattleCharacter* character);

	//! \brief Returns a pointer to the character that the player is selecting a command for
	BattleCharacter* GetCommandCharacter() const
		{ if (_active_settings == NULL) return NULL; else return _active_settings->GetCharacter(); }

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
	void NotifyActorDeath(private_battle::BattleActor* actor);

	//! \name Class member accessor methods
	//@{
	COMMAND_STATE GetState() const
		{ return _state; }

	BattleTarget GetSelectedTarget() const
		{ return _selected_target; }
	//@}

private:
	//! \brief The state that the action window is in, which reflects the contents of the window
	COMMAND_STATE _state;

	//! \brief A pointer to the settings for the active character
	CharacterCommandSettings* _active_settings;

	//! \brief A pointer to the skill that is currently selected, if any
	hoa_global::GlobalSkill* _selected_skill;

	//! \brief A pointer to the item that is currently selected, if any
	BattleItem* _selected_item;

	//! \brief Retains the target that the player has selected
	BattleTarget _selected_target;

	//! \brief An instance for managing the character party's inventory
	ItemCommand _item_command;

	//! \brief An instance for managing the skills of the active character
	SkillCommand _skill_command;

	//! \brief A container of setting objects for each character in the battle
	std::map<BattleCharacter*, CharacterCommandSettings> _character_settings;

	// ---------- Graphics and GUI members

	//! \brief Contains the icon images that represent each action category
	std::vector<hoa_video::StillImage> _category_icons;

	//! \brief Contains the text that represent each action category
	std::vector<hoa_video::TextImage> _category_text;

	//! \brief The window where all command information and GUI displays are drawn
	hoa_gui::MenuWindow _command_window;

	//! \brief Header text
	hoa_video::TextImage _window_header;

	//! \brief Rendered text that contains information about the currently selected target
	hoa_video::TextImage _window_text;

	/** \brief The option box that lists the types of actions that a character may take in battle
	*** Typically this list includes "attack", "defend", "support", and "item". More types may appear
	*** under special circumstances and conditions.
	**/
	hoa_gui::OptionBox _category_options;

	/** \brief Contains a list of the possible targets that a player may select from
	***
	*** This option box is used for the selection of both actors and attack points.
	**/
	hoa_gui::OptionBox _target_options;

	// ---------- Private methods

	//! \brief Returns true if the selected action category is a skill action
	bool _IsSkillCategorySelected() const;

	//! \brief Returns true if the selected action category is an item action
	bool _IsItemCategorySelected() const;

	//! \brief Returns the type of target for the selected action
	hoa_global::GLOBAL_TARGET _ActionTargetType();

	//! \brief Returns true if the character parameter has already had a settings instance created for it
	bool _HasCharacterSettings(BattleCharacter* character) const
		{ if (character == NULL) return false; else return (_character_settings.find(character) != _character_settings.end()); }

	/** \brief Creates and stores a CharacterCommandSettings instance for a given character
	*** \param character A pointer to the character to create the command settings for
	*** \note This function will not check whether or not an instance of CharacterCommandSettings has
	*** already been created for this character. That check should be done externally to this function
	*** call.
	**/
	void _CreateCharacterSettings(BattleCharacter* character)
		{ _character_settings.insert(std::make_pair(character, CharacterCommandSettings(character, _command_window))); }

	//! \brief Initializes the _selected_target member with an initial target for the currently selected action
	void _SetInitialTarget();

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

	//! \brief Updates state when the player is viewing information about an action
	void _UpdateInformation();

	//! \brief Draws visible contents to the screen when the player is selecting an action category
	void _DrawCategory();

	//! \brief Draws visible contents to the screen when the player is selecting an action
	void _DrawAction();

	//! \brief Draws visible contents to the screen when the player is selecting an actor target
	void _DrawActorTarget();

	//! \brief Draws visible contents to the screen when the player is selecting an attack point target
	void _DrawAttackPointTarget();

	//! \brief Draws visible contents to the screen when the player is viewing information about an action
	void _DrawInformation();

	//! \brief Sets the text for _window_header and _window_text to represent information about the selected target
	void _CreateActorTargetText();

	//! \brief Sets the text for _window_header and _window_text to represent information about the selected target
	void _CreateAttackPointTargetText();

	//! \brief Sets the text for _window_header and _window_text to represent information about the selected skill or item
	void _CreateInformationText();

	//! \brief Finalizes the player's command of the character by creating the appropriate action
	void _FinalizeCommand();
}; // class CommandSupervisor

} // namespace private_battle

} // namespace hoa_battle

#endif // __BATTLE_COMMAND_HEADER__
