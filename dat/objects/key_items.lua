------------------------------------------------------------------------------[[
-- Filename: items.lua
--
-- Description: This file contains the definitions of all items.
-- Each item has a unique integer identifier that is used
-- as its key in the items table below. Item IDs are unique not only among
-- each other, but among other inventory game objects as well (weapons, armor,
-- etc).
--
-- Object IDs 70,001 through 80,000 are reserved for key items. Do not break this
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Item IDs do -not- need to be sequential. When you make a new item, keep it
-- grouped with similar item types (potions, scrolls, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random items all over
-- this file.
--
-- All item entries needs the following data to be defined:
-- {name}: Text that defines the name of the item.
-- {description}: A brief description about the item.
-- {icon}: The filepath to the image icon representing this icon.
--
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (key_items == nil) then
	key_items = {}
end

--------------------------------------------------------------------------------
-- IDs 70001-80000 are reserved for key items
--------------------------------------------------------------------------------
key_items[70001] = {
	name = hoa_system.Translate("Pen"),
	description = hoa_system.Translate("Georges' pen, presumably used to write poetry."),
	icon = "img/icons/items/key_items/ink.png",
	standard_price = 0
}

key_items[70002] = {
	name = hoa_system.Translate("Barley Meal"),
	description = hoa_system.Translate("Basic yet good and cheap flour."),
	icon = "img/icons/items/key_items/barley_meal_bag.png",
	standard_price = 0
}
