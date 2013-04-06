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
-- {magical_attack}: The amount of magical damage that the weapon causes.
-- {standard_price}: The standard asking price of this weapon from merchants.
-- {usable_by}: A list of characters which may equip this weapon.
-- {slots}: The number of slots available to equip shards on the weapon.
-- {key_item}: Tells whether the item is a key item, preventing it from being consumed or sold.
------------------------------------------------------------------------------]]

-- All weapon definitions are stored in this table
if (weapons == nil) then
   weapons = {}
end


-- -----------------------------------------------------------------------------
-- IDs 10,001 - 10,500 are reserved for swords
-- -----------------------------------------------------------------------------

weapons[10001] = {
    name = vt_system.Translate("Wooden Sword"),
    description = vt_system.Translate("A sword made of wood with a steel hilt, very good for practising."),
    icon = "img/icons/weapons/woodensword.png",
    physical_attack = 2,
    magical_attack = 0,
    standard_price = 0,
    usable_by = BRONANN + THANIS,
    slots = 0,
    key_item = true,

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua"
        }
    }
}

weapons[10011] = {
    name = vt_system.Translate("Karlate Sword"),
    description = vt_system.Translate("Standard Karlate issued equipment. A light weight iron sword suitable for most skirmishes."),
    icon = "img/icons/weapons/karlate_sword.png",
    physical_attack = 12,
    magical_attack = 0,
    standard_price = 120,
    usable_by = BRONANN + THANIS,
    slots = 0,

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/steel/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10002] = {
    name = vt_system.Translate("Iron Sword"),
    description = vt_system.Translate("A sturdy but somewhat dull sword whose blade was forged from a single block of solid iron."),
    icon = "img/icons/weapons/iron_sword.png",
    physical_attack = 16,
    magical_attack = 0,
    standard_price = 250,
    usable_by = BRONANN + THANIS,
    slots = 0,

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/steel/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10003] = {
    name = vt_system.Translate("Knight's Blade"),
    description = vt_system.Translate("A weapon bestowed to seasoned veterans of the knighthood in several kingdoms throughout the world."),
    icon = "img/icons/weapons/knights_blade.png",
    physical_attack = 34,
    magical_attack = 0,
    standard_price = 800,
    usable_by = BRONANN + THANIS,
    slots = 1,

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/steel/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10004] = {
    name = vt_system.Translate("Paladin's Sword"),
    description = vt_system.Translate("A mythical weapon blessed with a magical fire."),
    icon = "img/icons/weapons/paladin-sword.png",
    physical_attack = 90,
    magical_attack = 30,
    standard_price = 30800,
    usable_by = BRONANN + THANIS,
    slots = 3,

    -- NOTE: Testing trade conditions, balance this later
    trade_conditions = {
        [0] = 20000,  -- price
        [10003] = 10, -- 10 Knight's blades
        [1003] = 5,   -- 5 Elixirs
        [1] = 3,      -- 3 minor healing potions
        [2] = 2,      -- 2 medium healing potions
        [3] = 6,      -- 6 healing potions
        [4] = 5,      -- 5 Mega healing potions
        [11] = 8      -- 8 Small moon juice
    },

    elemental_effects = {
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER
    },

    status_effects = {
        [vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
        [vt_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER
    },

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/steel/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

-- NOTE: Test weapon
weapons[10999] = {
    name = vt_system.Translate("Omni Sword"),
    description = "The ultimate sword, used only for testing...",
    icon = "img/icons/weapons/sword-flaming.png",
    physical_attack = 9999,
    magical_attack = 9999,
    standard_price = 9999999,
    usable_by = BRONANN + THANIS,
    slots = 5,
    key_item = true,

    -- NOTE: Testing trade conditions,
    trade_conditions = {
        [0] = 20000,  -- price
        [10003] = 10, -- 10 Knight's blades
        [1003] = 5,   -- 5 Elixirs
        [1] = 3,      -- 3 minor healing potions
        [2] = 2,      -- 2 medium healing potions
        [3] = 6,      -- 6 healing potions
        [4] = 5,      -- 5 Mega healing potions
        [11] = 8      -- 8 Small moon juice
    },

    elemental_effects = {
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_FIRE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_WATER] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_VOLT] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_EARTH] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_LIFE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_DEATH] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_ELEMENTAL_NEUTRAL] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME
    },

    status_effects = {
        [vt_global.GameGlobal.GLOBAL_STATUS_STRENGTH_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_PROTECTION_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_VIGOR_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_FORTITUDE_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_AGILITY_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_EVADE_RAISE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_HP_REGEN] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_SP_REGEN] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_PARALYSIS] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME
    },

    battle_animations = {
        [BRONANN] = {
            idle = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "img/sprites/battle/characters/bronann/weapons/steel/bronann_run.lua",
            attack = "img/sprites/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            dodge = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "img/sprites/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "img/sprites/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "img/sprites/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "img/sprites/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

-- -----------------------------------------------------------------------------
-- IDs 11,001 - 11,500 are reserved for arbalests
-- -----------------------------------------------------------------------------

weapons[11001] = {
    name = vt_system.Translate("Arbalest"),
    description = vt_system.Translate("A standard wooden arbalest."),
    icon = "img/icons/weapons/arbalest.png",
    -- The image displayed when kalya fires with her arbalest.
    battle_ammo_animation_file = "img/sprites/battle/ammo/wood_arrow.lua",
    physical_attack = 3,
    magical_attack = 0,
    standard_price = 50,
    usable_by = KALYA,
    slots = 0,
    key_item = true,

    battle_animations = {
        [KALYA] = {
            idle = "img/sprites/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            run = "img/sprites/battle/characters/kalya/weapons/steel/kalya_run.lua",
            attack = "img/sprites/battle/characters/kalya/weapons/steel/kalya_attack.lua",
            dodge = "img/sprites/battle/characters/kalya/weapons/steel/kalya_dodge.lua",
            victory = "img/sprites/battle/characters/kalya/weapons/steel/kalya_victory.lua",
            hurt = "img/sprites/battle/characters/kalya/weapons/steel/kalya_hurt.lua",
            poor = "img/sprites/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dying = "img/sprites/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dead = "img/sprites/battle/characters/kalya/weapons/steel/kalya_dead.lua",
            revive = "img/sprites/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            item = "img/sprites/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            magic_prepare = "img/sprites/battle/characters/kalya/weapons/steel/kalya_magic_prepare.lua",
            magic_cast = "img/sprites/battle/characters/kalya/weapons/steel/kalya_magic_cast.lua"
        }
    }
}
