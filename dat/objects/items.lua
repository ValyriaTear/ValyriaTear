------------------------------------------------------------------------------[[
-- Filename: items.lua
--
-- Description: This file contains the definitions of all items that exist in
-- Hero of Allacrost. Each item has a unique integer identifier that is used
-- as its key in the items table below. Item IDs are unique not only among
-- each other, but among other inventory game objects as well (weapons, armor,
-- etc).
--
-- Object IDs 1 through 10,000 are reserved for items. Do not break this 
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
-- {target_type}: The type of target the item affects, which may be an attack point, actor, or party.
-- {standard_price}: The standard asking price of this weapon from merchants.
--
-- Each item entry requires a function called {BattleUse} to be defined. This function implements the
-- use of item in battle, healing damage, causing status changes, playing sounds, and animating
-- sprites.
--
-- If an item is to be permitted for use outside of battle, an additional function {FieldUse} should
-- be defined. Defintion of this function is optional, but required for any items which are intended
-- to be used in the field. Their implementations will be similar to but more simple than their BattleUse
-- cousins as they do not need to be concerned with certain things such as sprite animations.
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (items == nil) then
	items = {}
end


--------------------------------------------------------------------------------
-- IDs 1 - 1,000 are reserved for healing potions
--------------------------------------------------------------------------------

items[1] = {
	name = hoa_system.Translate("Minor Healing Potion"),
	description = hoa_system.Translate("Restores a small amount of hit points to a target."),
	icon = "img/icons/items/potion_green_small.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 60,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(45);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddHitPoints(45);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

--------------------------------------------------------------------------------
-- IDs 1,001 - 2,000 are reserved for status potions
--------------------------------------------------------------------------------

items[1001] = {
	name = hoa_system.Translate("Minor Elixir"),
	description = hoa_system.Translate("Reduces ailing status effects by a limited degree."),
	icon = "img/icons/items/potion_blue_small.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 60,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		-- TODO: decrement any active negative status effects
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

--------------------------------------------------------------------------------
-- IDs 2,001 - 3,000 are reserved for elemental potions
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- IDs 3,001 - 4,000 are reserved for attack items
--------------------------------------------------------------------------------


