------------------------------------------------------------------------------[[
-- Filename: weapons.lua
--
-- Description: This file contains the definitions of all weapons that exist in
-- Hero of Allacrost. Each weapon has a unique integer identifier that is used
-- as its key in the weapons table below. Weapon IDs are unique not only among
-- each other, but among other inventory game objects as well (items, armor,
-- etc).
--
-- Object IDs 10,001 through 20,000 are reserved for weapons. Do not break this 
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Weapons IDs do -not- need to be sequential. When you make a new weapon, keep it 
-- grouped with similar weapon types (swords with swords, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random weapons all over
-- this file.
--
-- All weapon entries need the following data to be defined:
-- {name}: Text that defines the name of the weapon.
-- {description}: A brief description about the weapon.
-- {icon}: The filepath to the image icon representing this weapon.
-- {physical_attack}: The amount of physical damage that the weapon causes.
-- {metaphysical_attack}: The amount of metaphysical damage that the weapon causes.
-- {standard_price}: The standard asking price of this weapon from merchants.
-- {usable_by}: A list of characters which may equip this weapon.
-- {slots}: The number of slots available to equip shards on the weapon.
------------------------------------------------------------------------------]]

-- All weapon definitions are stored in this table
if (weapons == nil) then
   weapons = {}
end


-- -----------------------------------------------------------------------------
-- IDs 10,001 - 10,500 are reserved for swords
-- -----------------------------------------------------------------------------

weapons[10001] = {
	name = hoa_system.Translate("Karlate Sword"),
	description = hoa_system.Translate("Standard Karlate issued equipment. A light weight iron sword suitable for most skirmishes."),
	icon = "img/icons/weapons/karlate_sword.png",
	physical_attack = 12,
	metaphysical_attack = 0,
	standard_price = 120,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

weapons[10002] = {
	name = hoa_system.Translate("Iron Sword"),
	description = hoa_system.Translate("A sturdy but somewhat dull sword whose blade was forged from a single block of solid iron."),
	icon = "img/icons/weapons/iron_sword.png",
	physical_attack = 16,
	metaphysical_attack = 0,
	standard_price = 250,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

weapons[10003] = {
	name = hoa_system.Translate("Knight's Blade"),
	description = hoa_system.Translate("A weapon bestowed to seasoned veterans of the knighthood in several kingdoms throughout the world."),
	icon = "img/icons/weapons/knights_blade.png",
	physical_attack = 34,
	metaphysical_attack = 0,
	standard_price = 800,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 1
}

