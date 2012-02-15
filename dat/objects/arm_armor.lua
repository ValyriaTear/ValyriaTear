------------------------------------------------------------------------------[[
-- Filename: arm_armor.lua
--
-- Description: This file contains the definitions of all arm armor that exist in
-- Hero of Allacrost. Each armor has a unique integer identifier that is used
-- as its key in the armor table below. Armor IDs are unique not only among
-- each other, but among other inventory game objects as well (items, weapons,
-- other classes of armor, etc).
--
-- Object IDs 40,001 through 50,000 are reserved for arm armor. Do not break this 
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Armor IDs do -not- need to be sequential. When you make a new armor, keep it 
-- grouped with similar armor types (shields with shields, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random arm armor all over
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
-- IDs 40,001 - 40,500 are reserved for shields
-- -----------------------------------------------------------------------------

armor[40001] = {
	name = hoa_system.Translate("Wooden Shield"),
	description = hoa_system.Translate("Robust wooden oak protects from all but the heaviest of assaults."),
	icon = "img/icons/armor/wooden_shield.png",
	physical_defense = 2,
	metaphysical_defense = 0,
	standard_price = 90,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

armor[40002] = {
	name = hoa_system.Translate("Metal Shield"),
	description = hoa_system.Translate("A shield formed completely out of a light-weight but cheap metallic alloy."),
	icon = "img/icons/armor/metal_shield.png",
	physical_defense = 5,
	metaphysical_defense = 2,
	standard_price = 170,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

armor[40003] = {
	name = hoa_system.Translate("Phoenix Shield"),
	description = hoa_system.Translate("A tall steel shield with a mighty phoenix emblazoned on the front."),
	icon = "img/icons/armor/phoenix_shield.png",
	physical_defense = 10,
	metaphysical_defense = 6,
	standard_price = 310,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

armor[40004] = {
	name = hoa_system.Translate("Trident Shield"),
	description = hoa_system.Translate("A large shield with enough fortitude to withstand waves of powerful attacks."),
	icon = "img/icons/armor/trident_shield.png",
	physical_defense = 10,
	metaphysical_defense = 6,
	standard_price = 310,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 1
}

