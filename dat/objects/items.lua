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
-- {warmup_time}: The time needed before using that item in battles.
-- {cooldown_time}: The time needed after using that item in battles.
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
	description = hoa_system.Translate("Restores a small amount of hit points to an ally."),
	icon = "img/icons/items/potion_green_small.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 60,
	warmup_time = 1000,
	cooldown_time = 700,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(40, true);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddHitPoints(45);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[2] = {
	name = hoa_system.Translate("Medium Healing Potion"),
	description = hoa_system.Translate("Restores a reasonable amount of hit points to an ally."),
	icon = "img/icons/items/potion_green_medium.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 300,
	warmup_time = 1200,
	cooldown_time = 900,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(150, true);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddHitPoints(200);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[3] = {
	name = hoa_system.Translate("Healing Potion"),
	description = hoa_system.Translate("Restores a large amount of hit points to an ally."),
	icon = "img/icons/items/potion_green_large.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 1000,
	warmup_time = 1200,
	cooldown_time = 900,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(500, true);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddHitPoints(620);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[4] = {
	name = hoa_system.Translate("Mega Healing Potion"),
	description = hoa_system.Translate("Restores a very high amount of hit points to an ally."),
	icon = "img/icons/items/potion_green_large.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 5000,
	warmup_time = 1200,
	cooldown_time = 1200,

	BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(9000, true);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddHitPoints(12000);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

-- Moon juices : Skill points
items[11] = {
	name = hoa_system.Translate("Small Moon Juice Potion"),
	description = hoa_system.Translate("Restores a small amount of skill points to an ally."),
	icon = "img/icons/items/potion_blue_small.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 90,
	use_warmup_time = 1000,
	cooldown_time = 1200,

    BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(40, false);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddSkillPoints(45);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[12] = {
	name = hoa_system.Translate("Medium Moon Juice Potion"),
	description = hoa_system.Translate("Restores a reasonable amount of skill points to an ally."),
	icon = "img/icons/items/potion_blue_medium.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 380,
	use_warmup_time = 1000,
	cooldown_time = 1200,

    BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(150, false);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddSkillPoints(200);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[13] = {
	name = hoa_system.Translate("Moon Juice Potion"),
	description = hoa_system.Translate("Restores a large amount of skill points to an ally."),
	icon = "img/icons/items/potion_blue_large.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 1300,
	use_warmup_time = 1000,
	cooldown_time = 1200,

    BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(300, false);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddSkillPoints(420);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

items[13] = {
	name = hoa_system.Translate("Mega Moon Juice Potion"),
	description = hoa_system.Translate("Restores a very high amount of skill points to an ally."),
	icon = "img/icons/items/potion_blue_huge.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 6100,
	use_warmup_time = 1200,
	cooldown_time = 1300,

    BattleUse = function(user, target)
		target_actor = target:GetActor();
		target_actor:RegisterHealing(999, false);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end,

	FieldUse = function(target)
		target:AddSkillPoints(1200);
		AudioManager:PlaySound("snd/potion_drink.wav");
	end
}

--------------------------------------------------------------------------------
-- IDs 1,001 - 2,000 are reserved for status potions
--------------------------------------------------------------------------------

items[1001] = {
	name = hoa_system.Translate("Minor Elixir"),
	description = hoa_system.Translate("Reduces ailing status effects by a limited degree."),
	icon = "img/icons/items/potion_red_small.png",
	target_type = hoa_global.GameGlobal.GLOBAL_TARGET_ALLY,
	standard_price = 160,
	use_warmup_time = 1200,
	cooldown_time = 1800,

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


