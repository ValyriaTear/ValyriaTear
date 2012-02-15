////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_objects.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for global game objects
***
*** This file contains several representations of inventory "objects" used
*** throughout the game. Objects include items, weapons, armor, etc.
*** ***************************************************************************/

#ifndef __GLOBAL_OBJECTS_HEADER__
#define __GLOBAL_OBJECTS_HEADER__

#include "defs.h"
#include "utils.h"

#include "script.h"
#include "video.h"

#include "global_utils.h"

namespace hoa_global {

/** ****************************************************************************
*** \brief An abstract base class for representing a game object
***
*** All game objects inherit from this class. This allows objects of all types to
*** be stored in the same container (an inventory list for instance) and promotes
*** efficient code reuse for all game objects. The class is designed so that a
*** single class object can represent multiple instances of the same game object.
*** In other words, you can represent 50 healing potions with a single GlobalObject
*** class object rather than having to create and managed 50 class objects, one for
*** each potion. The _count member achieves this convenient function.
***
*** A GlobalObject with an ID value of zero is considered invalid. Most of the
*** protected members of this class can only be set by the constructors or methods
*** of deriving classes.
***
*** \note The price of an object is not actually the price it is bought or sold
*** at in the game. It is a "base price" from which all levels of buy and sell
*** prices are derived from.
***
*** \todo The "lore" for an object is a feature that we have discussed but not
*** yet decided if we wish to implement. Placeholders exist in this class for now,
*** but if lore is not to be implemented as a game feature they should be removed.
*** ***************************************************************************/
class GlobalObject {
public:
	GlobalObject() :
		_id(0), _count(0), _price(0) {}

	GlobalObject(uint32 id, uint32 count = 1) :
		_id(id), _count(count), _price(0) {}

	virtual ~GlobalObject()
		{}

	//! \brief Returns true if the object is properly initialized and ready to be used
	bool IsValid() const
		{ return (_id != 0); }

	/** \brief Purely virtual function used to distinguish between object types
	*** \return A value that represents the type of object
	**/
	virtual GLOBAL_OBJECT GetObjectType() const = 0;

	/** \brief Increments the number of objects represented by this class
	*** \param count The count increment value (default value == 1)
	**/
	void IncrementCount(uint32 count = 1)
		{ _count += count; }

	/** \brief Decrements the number of objects represented by this class
	*** \param count The count decrement value (default value == 1)
	*** \note When the count reaches zero, this class object does <i>not</i> self-destruct. It is the user's
	*** responsiblity to check if the count becomes zero, and to destroy the object if it is appropriate to do so.
	**/
	void DecrementCount(uint32 count = 1)
		{ if (count > _count) _count = 0; else _count -= count; }

	//! \name Class Member Access Functions
	//@{
	uint32 GetID() const
		{ return _id; }

	const hoa_utils::ustring& GetName() const
		{ return _name; }

	const hoa_utils::ustring& GetDescription() const
		{ return _description; }

	const hoa_utils::ustring& GetLore() const
		{ return _lore; }

	void SetCount(uint32 count)
		{ _count = count; }

	uint32 GetCount() const
		{ return _count; }

	uint32 GetPrice() const
		{ return _price; }

	const hoa_video::StillImage& GetIconImage() const
		{ return _icon_image; }
	//@}

protected:
	/** \brief An identification number for each unique item
	*** \note An ID number of zero indicates an invalid object
	**/
	uint32 _id;

	//! \brief The name of the object as it would be displayed on a screen
	hoa_utils::ustring _name;

	//! \brief A short description of the item to display on the screen
	hoa_utils::ustring _description;

	//! \brief A detailed description of the object's history, culture, and how it fits into the game world
	hoa_utils::ustring _lore;

	//! \brief Retains how many occurences of the object are represented by this class object instance
	uint32 _count;

	//! \brief The base price of the object for purchase/sale in the game
	uint32 _price;

	//! \brief A loaded icon image of the object at its original size of 60x60 pixels
	hoa_video::StillImage _icon_image;

	//! \brief Causes the object to become invalid due to a loading error or other significant issue
	void _InvalidateObject()
		{ _id = 0; }

	/** \brief Reads object data from an open script file
	*** \param script A reference to a script file that has been opened and prepared
	***
	*** This method does not do any of its own error case checking. Only dervied classes may call this
	*** protected function and they are expected to have the script file successfully opened and the correct
	*** table context prepared. This function will do nothing more but read the expected key/values of
	*** the open table in the script file and return.
	**/
	void _LoadObjectData(hoa_script::ReadScriptDescriptor& script);
}; // class GlobalObject


/** ****************************************************************************
*** \brief Represents items used throughout the game
***
*** This class is for general use items such as healing potions. Each item has a
*** different effect when it is used, implemented by a Lua function written
*** specifically for the item which calls it. Some items may be used only in certain
*** scenarios (in battles, on the field, etc.). All items may be used by any
*** character or enemy in the game.
*** ***************************************************************************/
class GlobalItem : public GlobalObject {
public:
	/** \param id The unique ID number of the item
	*** \param count The number of items to initialize this class object as representing (default value == 1)
	**/
	GlobalItem(uint32 id, uint32 count = 1);

	~GlobalItem();

	GlobalItem(const GlobalItem& copy);

	GlobalItem& operator=(const GlobalItem& copy);

	GLOBAL_OBJECT GetObjectType() const
		{ return GLOBAL_OBJECT_ITEM; }

	//! \brief Returns true if the item can be used in battle
	bool IsUsableInBattle()
		{ return (_battle_use_function != NULL); }

	//! \brief Returns true if the item can be used in the field
	bool IsUsableInField()
		{ return (_field_use_function != NULL); }

	//! \name Class Member Access Functions
	//@{
	GLOBAL_TARGET GetTargetType() const
		{ return _target_type; }

	/** \brief Returns a pointer to the ScriptObject of the battle use function
	*** \note This function will return NULL if the skill is not usable in battle
	**/
	const ScriptObject* GetBattleUseFunction() const
		{ return _battle_use_function; }

	/** \brief Returns a pointer to the ScriptObject of the field use function
	*** \note This function will return NULL if the skill is not usable in the field
	**/
	const ScriptObject* GetFieldUseFunction() const
		{ return _field_use_function; }
	//@}

private:
	//! \brief The type of target for the item
	GLOBAL_TARGET _target_type;

	//! \brief A pointer to the script function that performs the item's effect while in battle
	ScriptObject* _battle_use_function;

	//! \brief A pointer to the script function that performs the item's effect while in a menu
	ScriptObject* _field_use_function;
}; // class GlobalItem : public GlobalObject


/** ****************************************************************************
*** \brief Represents weapon that may be equipped by characters or enemies
***
*** All classes of weapons (swords, bows, spears, etc.) are represented by this
*** class. Typically, a weapon may only be used by a select few and can not be
*** equipped on every character. Weapons have two attack ratings: physical
*** and metaphysical, both of which are included in the damage calculation
*** formulae when a character or enemy attacks using the weapon. Weapons may also
*** have a small number of "sockets" in which shards can be inserted to improve
*** or alter the weapon's properties. Some weapons have zero sockets available.
*** Finally, weapons may come imbued with certain elemental or status effect
*** properties that are inflicted on a target.
*** ***************************************************************************/
class GlobalWeapon : public GlobalObject {
public:
	/** \param id The unique ID number of the weapon
	*** \param count The number of weapons to initialize this class object as representing (default value == 1)
	**/
	GlobalWeapon(uint32 id, uint32 count = 1);

	~GlobalWeapon()
		{}

	GLOBAL_OBJECT GetObjectType() const
		{ return GLOBAL_OBJECT_WEAPON; }

	//! \name Class Member Access Functions
	//@{
	uint32 GetPhysicalAttack() const
		{ return _physical_attack; }

	uint32 GetMetaphysicalAttack() const
		{ return _metaphysical_attack; }

	uint32 GetUsableBy() const
		{ return _usable_by; }

	const std::vector<GlobalShard*>& GetSockets() const
		{ return _sockets; }

	const std::map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>& GetElementalEffects() const
		{ return _elemental_effects; }
	//@}

private:
	//! \brief The amount of physical damage that the weapon causes
	uint32 _physical_attack;

	//! \brief The amount of metaphysical damage that the weapon causes
	uint32 _metaphysical_attack;

	/** \brief A bit-mask that determines which characters can use or equip the object
	*** See the game character ID constants in global_actors.h for more information
	**/
	uint32 _usable_by;

	/** \brief Sockets which may be used to place shards on the weapon
	*** Weapons may have no sockets, so it is not uncommon for the size of this vector to be zero.
	*** When a socket is available but empty (has no attached shard), the pointer at that index
	*** will be NULL.
	**/
	std::vector<GlobalShard*> _sockets;

	/** \brief Container that holds the intensity of each type of elemental effect of the weapon
	*** Elements with an intensity of GLOBAL_INTENSITY_NEUTRAL indicate no elemental bonus
	**/
	std::map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY> _elemental_effects;

	// TODO: Add status effects to weapons
	// std::map<GLOBAL_STATUS, GLOBAL_INTENSITY> _status_effects;
}; // class GlobalWeapon : public GlobalObject


/** ****************************************************************************
*** \brief Represents all types of armor that may be equipped on characters and enemies
***
*** There are actually four types of armor: head, torso, arm, and leg. However all
*** four types are represented by this single class. The only functional difference
*** between different types of armor is where they may be equipped on an actor. Not
*** all armor can be equipped by any character or enemy. Typically, armor may only
*** be used by a select few and can not be equipped on every character. Armor have
*** two defense ratings: physical and metaphysical, both of which are included in
*** the damage calculation formulae when a character or enemy is attacked at the
*** location where the armor is equipped. Armor may also have a small number of
*** "sockets" in which shards can be inserted to improve or alter the armor's
*** properties. Some armor will have zero sockets available. Finally, armor may
*** come imbued with certain elemental or status effect properties that bolster
*** and protect the user.
*** ***************************************************************************/
class GlobalArmor : public GlobalObject {
public:
	GlobalArmor(uint32 id, uint32 count = 1);

	~GlobalArmor()
		{}

	//! \brief Returns the approriate armor type (head, torso, arm, leg) depending on the object ID
	GLOBAL_OBJECT GetObjectType() const;

	uint32 GetPhysicalDefense() const
		{ return _physical_defense; }

	uint32 GetMetaphysicalDefense() const
		{ return _metaphysical_defense; }

	uint32 GetUsableBy() const
		{ return _usable_by; }

	const std::vector<GlobalShard*>& GetSockets() const
		{ return _sockets; }

	const std::map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY>& GetElementalEffects() const
		{ return _elemental_effects; }

private:
	//! \brief The amount of physical defense that the armor provides
	uint32 _physical_defense;

	//! \brief The amount of metaphysical defense that the armor provides
	uint32 _metaphysical_defense;

	/** \brief A bit-mask that determines which characters can use or equip the object
	*** See the game character ID constants in global_actors.h for more information
	**/
	uint32 _usable_by;

	/** \brief Sockets which may be used to place shards on the armor
	*** Armor may have no sockets, so it is not uncommon for the size of this vector to be zero.
	*** When a socket is available but empty (has no attached shard), the pointer at that index
	*** will be NULL.
	**/
	std::vector<GlobalShard*> _sockets;

	/** \brief Container that holds the intensity of each type of elemental effect of the armor
	*** Elements with an intensity of GLOBAL_INTENSITY_NEUTRAL indicate no elemental bonus
	**/
	std::map<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY> _elemental_effects;

	// TODO: Add status effects to weapons
	// std::map<GLOBAL_STATUS, GLOBAL_INTENSITY> _status_effects;
}; // class GlobalArmor : public GlobalObject


/** ****************************************************************************
*** \brief Represents any type of shard that can be attached to weapons and armor
***
*** Shards are small gems or stones that can be placed into sockets available on
*** weapons and armor. Shards have the ability to enhance the properties of
*** equipment it is attached to, allowing the player a degree of customization
*** in the weapons and armor that their character use.
***
*** \todo This class is not yet implemented
*** ***************************************************************************/
class GlobalShard : public GlobalObject {
public:
	GlobalShard(uint32 id, uint32 count = 1);

	GLOBAL_OBJECT GetObjectType() const
		{ return GLOBAL_OBJECT_SHARD; }
}; // class GlobalShard : public GlobalObject


/** ****************************************************************************
*** \brief Represents key items found throughout the game
***
*** Key items are special items which can not be used directly used nor sold by
*** the player. Their primary function is to for use in game logic. For example,
*** a copper key may be needed to open a certain door in a dungeon. Key items
*** have no need for further data  nor logic beyond what is provided in the
*** GlobalObject base class is necessary for key items.
*** ***************************************************************************/
class GlobalKeyItem : public GlobalObject {
public:
	GlobalKeyItem(uint32 id, uint32 count = 1);

	GLOBAL_OBJECT GetObjectType() const
		{ return GLOBAL_OBJECT_KEY_ITEM; }
}; // class GlobalKeyItem : public GlobalObject

} // namespace hoa_global

#endif // __GLOBAL_OBJECTS_HEADER__
