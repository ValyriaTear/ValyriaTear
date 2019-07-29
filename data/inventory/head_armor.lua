------------------------------------------------------------------------------[[
-- Filename: head_armor.lua
--
-- Description: This file contains the definitions of all head armor.
-- Each armor has a unique integer identifier that is used
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
-- {magical_defense}: The amount of magical defense that the armor casues.
-- {standard_price}: The standard asking price of this armor from merchants.
-- {usable_by}: A list of characters which may equip this armor,
-- {slots}: The number of slots available to equip shards on the armor.
-- {key_item}: Tells whether the item is a key item, preventing it from being consumed or sold.
------------------------------------------------------------------------------]]

-- All armor definitions are stored in this table
if (armor == nil) then
   armor = {}
end


-- -----------------------------------------------------------------------------
-- IDs 20,001 - 20,500 are reserved for helmets
-- -----------------------------------------------------------------------------

armor[20001] = {
	name = vt_system.Translate("Butterfly Hair Tong"),
	description = vt_system.Translate("An old and finely crafted piece of jewelery."),
	icon = "data/inventory/armor/butterfly_hair_tong.png",
	physical_defense = 3,
	magical_defense = 12,
	standard_price = 450,
	usable_by = KALYA,
    key_item = true
}

armor[20002] = {
    name = vt_system.Translate("Wolfpain Necklace"),
    description = vt_system.Translate("A strange necklace with unknown powers."),
    icon = "data/inventory/items/key_items/necklace_wolfpain.png",
    physical_defense = 8,
    magical_defense = 24,
    standard_price = 0,
    key_item = true,
    usable_by = KALYA,
    status_effects = {
        [vt_global.GameGlobal.GLOBAL_STATUS_SP] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
    },
}

armor[20011] = {
    name = vt_system.Translate("Old Helmet"),
    description = vt_system.Translate("A battle-worn helmet, still reliable for head protection."),
    icon = "data/inventory/armor/karlate_helmet.png",
    physical_defense = 24,
    magical_defense = 12,
    standard_price = 80,
    usable_by = BRONANN + KALYA + THANIS + SYLVE,
    slots = 0
}

armor[20012] = {
	name = vt_system.Translate("Cobalt Helm"),
	description = vt_system.Translate("A small helmet composed of a secret cobalt-based alloy."),
	icon = "data/inventory/armor/cobalt_helm.png",
	physical_defense = 8,
	magical_defense = 4,
	standard_price = 140,
	usable_by = BRONANN + KALYA + THANIS + SYLVE,
	slots = 0
}

armor[20013] = {
	name = vt_system.Translate("Crested Helmet"),
	description = vt_system.Translate("A sturdy helmet with a hinged face guard offering additional protection."),
	icon = "data/inventory/armor/crested_helmet.png",
	physical_defense = 11,
	magical_defense = 6,
	standard_price = 220,
	usable_by = BRONANN + KALYA + THANIS + SYLVE,
	slots = 0
}
