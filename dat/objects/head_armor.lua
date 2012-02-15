------------------------------------------------------------------------------[[
-- Filename: head_armor.lua
--
-- Description: This file contains the definitions of all head armor that exist in
-- Hero of Allacrost. Each armor has a unique integer identifier that is used
-- as its key in the armor table below. Armor IDs are unique not only among
-- each other, but among other inventory game objects as well (items, weapons,
-- other classes of armor, etc).
--
-- Object IDs 20,001 through 30,000 are reserved for head armor. Do not break this 
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Armor IDs do -not- need to be sequential. When you make a new armor, keep it 
-- grouped with similar armor types (helmets with helmets, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random head armor all over
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
-- IDs 20,001 - 20,500 are reserved for helmets
-- -----------------------------------------------------------------------------

armor[20001] = {
	name = hoa_system.Translate("Karlate Helmet"),
	description = hoa_system.Translate("Standard Karlate issued equipment. Battle worn but reliable head protection."),
	icon = "img/icons/armor/karlate_helmet.png",
	physical_defense = 6,
	metaphysical_defense = 2,
	standard_price = 80,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

armor[20002] = {
	name = hoa_system.Translate("Cobalt Helm"),
	description = hoa_system.Translate("A small helmet composed of a secret cobalt based alloy."),
	icon = "img/icons/armor/cobalt_helm.png",
	physical_defense = 8,
	metaphysical_defense = 4,
	standard_price = 140,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

armor[20003] = {
	name = hoa_system.Translate("Crested Helmet"),
	description = hoa_system.Translate("A sturdy helmet with a hinged face guard offering additional protection."),
	icon = "img/icons/armor/crested_helmet.png",
	physical_defense = 11,
	metaphysical_defense = 6,
	standard_price = 220,
	usable_by = CLAUDIUS + MARK + DESTER + LUKAR,
	slots = 0
}

