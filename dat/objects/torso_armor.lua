------------------------------------------------------------------------------[[
-- Filename: torso_armor.lua
--
-- Description: This file contains the definitions of all torso armor.
-- Each armor has a unique integer identifier that is used
-- as its key in the armor table below. Armor IDs are unique not only among
-- each other, but among other inventory game objects as well (items, weapons,
-- other classes of armor, etc).
--
-- Object IDs 30,001 through 40,000 are reserved for torso armor. Do not break this
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Armor IDs do -not- need to be sequential. When you make a new armor, keep it
-- grouped with similar armor types (plate mail with plate mail, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random torso armor all over
-- this file.
--
-- All armor entries need the following data to be defined:
-- {name}: Text that defines the name of the armor.
-- {description}: A brief description about the armor.
-- {icon}: The filepath to the image icon representing this armor.
-- {physical_defense}: The amount of physical defense that the armor provides.
-- {metaphysical_defense}: The amount of metaphysical defense that the armor casues.
-- {standard_price}: The standard asking price of this armor from merchants.
-- {usable_by}: A list of characters which may equip this armor,
-- {slots}: The number of slots available to equip shards on the armor.
------------------------------------------------------------------------------]]

-- All armor definitions are stored in this table
if (armor == nil) then
   armor = {}
end


-- -----------------------------------------------------------------------------
-- IDs 30,001 - 30,500 are reserved for plate mail
-- -----------------------------------------------------------------------------

armor[30001] = {
	name = hoa_system.Translate("Rookie Tunic"),
	description = hoa_system.Translate("A light soldier rookie tunic worn by new recruits. That one belonged to Bronann's father..."),
	icon = "img/icons/armor/rookie_tunic.png",
	physical_defense = 1,
	metaphysical_defense = 0,
	standard_price = 0,
	usable_by = BRONANN
}

armor[30002] = {
	name = hoa_system.Translate("Old Willow Dress"),
	description = hoa_system.Translate("An old Willow dress Kalya likes to wear."),
	icon = "img/icons/armor/willow_dress.png",
	physical_defense = 1,
	metaphysical_defense = 0,
	standard_price = 0,
	usable_by = KALYA
}

armor[30003] = {
	name = hoa_system.Translate("Tunic"),
	description = hoa_system.Translate("An old light soldier tunic."),
	icon = "img/icons/armor/green_tunic.png",
	physical_defense = 5,
	metaphysical_defense = 0,
	standard_price = 60,
	usable_by = BRONANN
}

armor[30004] = {
	name = hoa_system.Translate("Leather cloak"),
	description = hoa_system.Translate("A cloak worn by apprentices."),
	icon = "img/icons/armor/cloak_leather_brown.png",
	physical_defense = 5,
	metaphysical_defense = 2,
	standard_price = 80,
	usable_by = KALYA
}

armor[30011] = {
	name = hoa_system.Translate("Leather Chain Mail"),
	description = hoa_system.Translate("A light chain mail woven into a tough leather cuirass."),
	icon = "img/icons/armor/leather_chain_mail.png",
	physical_defense = 8,
	metaphysical_defense = 2,
	standard_price = 170,
	usable_by = BRONANN + KALYA + THANIS + SYLVE,
	slots = 0
}

armor[30012] = {
	name = hoa_system.Translate("Karlate Breastplate"),
	description = hoa_system.Translate("Standard Karlate issued equipment. Effectively protects the torso from most types of attack."),
	icon = "img/icons/armor/karlate_breastplate.png",
	physical_defense = 14,
	metaphysical_defense = 3,
	standard_price = 320,
	usable_by = BRONANN + KALYA + THANIS + SYLVE,
	slots = 0
}

armor[30013] = {
	name = hoa_system.Translate("Shouldered Breastplate"),
	description = hoa_system.Translate("A breastplate with slightly thicker armor than is commonly found and additional shoulder guards."),
	icon = "img/icons/armor/shouldered_breastplate.png",
	physical_defense = 20,
	metaphysical_defense = 5,
	standard_price = 600,
	usable_by = BRONANN + THANIS,
	slots = 0
}
