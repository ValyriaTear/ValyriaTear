------------------------------------------------------------------------------[[
-- Filename: weapons.lua
--
-- Description: This file contains the definitions of all weapons.
-- Each weapon has a unique integer identifier that is used
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
	name = hoa_system.Translate("Wooden Sword"),
	description = hoa_system.Translate("A sword made of wood and a steel hilt, very good for practising."),
	icon = "img/icons/weapons/woodensword.png",
	physical_attack = 2,
	metaphysical_attack = 0,
	standard_price = 30,
	usable_by = BRONANN + THANIS,
	slots = 0
}

weapons[10011] = {
	name = hoa_system.Translate("Karlate Sword"),
	description = hoa_system.Translate("Standard Karlate issued equipment. A light weight iron sword suitable for most skirmishes."),
	icon = "img/icons/weapons/karlate_sword.png",
	physical_attack = 12,
	metaphysical_attack = 0,
	standard_price = 120,
	usable_by = BRONANN + THANIS,
	slots = 0
}

weapons[10002] = {
	name = hoa_system.Translate("Iron Sword"),
	description = hoa_system.Translate("A sturdy but somewhat dull sword whose blade was forged from a single block of solid iron."),
	icon = "img/icons/weapons/iron_sword.png",
	physical_attack = 16,
	metaphysical_attack = 0,
	standard_price = 250,
	usable_by = BRONANN + THANIS,
	slots = 0
}

weapons[10003] = {
	name = hoa_system.Translate("Knight's Blade"),
	description = hoa_system.Translate("A weapon bestowed to seasoned veterans of the knighthood in several kingdoms throughout the world."),
	icon = "img/icons/weapons/knights_blade.png",
	physical_attack = 34,
	metaphysical_attack = 0,
	standard_price = 800,
	usable_by = BRONANN + THANIS,
	slots = 1
}

weapons[10004] = {
	name = hoa_system.Translate("Paladin's Sword"),
	description = hoa_system.Translate("A mythical weapon blessed with a magical fire."),
	icon = "img/icons/weapons/paladin-sword.png",
	physical_attack = 90,
	metaphysical_attack = 30,
	standard_price = 30800,
	usable_by = BRONANN + THANIS,
	slots = 3,

	elemental_effects = {
		[hoa_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE] = hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER
	},

	status_effects = {
		[hoa_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE] = hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
		[hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE] = hoa_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER
	}
}

-- -----------------------------------------------------------------------------
-- IDs 11,001 - 11,500 are reserved for arbalests
-- -----------------------------------------------------------------------------

weapons[11001] = {
	name = hoa_system.Translate("Arbalest"),
	description = hoa_system.Translate("A standard wooden arbalest."),
	icon = "img/icons/weapons/arbalest.png",
	-- The image displayed when kalya fires with her arbalest.
	battle_ammo_animation_file = "img/sprites/battle/ammo/wood_arrow.lua",
	physical_attack = 3,
	metaphysical_attack = 0,
	standard_price = 50,
	usable_by = KALYA,
	slots = 0
}