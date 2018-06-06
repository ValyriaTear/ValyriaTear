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

-- Wood/earth family
-- -----------------
weapons[10001] = {
    name = vt_system.Translate("Wooden Sword"),
    description = vt_system.Translate("A sword made of wood with a steel hilt, very good for practising."),
    icon = "data/inventory/weapons/woodensword.png",
    physical_attack = 2,
    magical_attack = 0,
    standard_price = 0,
    usable_by = BRONANN + THANIS,
    slots = 0,
    key_item = true,

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua"
        }
    }
}

weapons[10002] = {
    name = vt_system.Translate("Improved Wooden Sword"),
    description = vt_system.Translate("A sword made of wood with a steel hilt, very good for practising."),
    icon = "data/inventory/weapons/woodensword.png",
    physical_attack = 15,
    magical_attack = 0,
    standard_price = 0,
    usable_by = BRONANN + THANIS,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 35,    -- Drunes
        [3119] = 3,  -- Fine Wood
        [3112] = 1,  -- Silk
        [3101] = 1,  -- Refined Slimy Material
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua"
        }
    }
}

weapons[10003] = {
    name = vt_system.Translate("Reinforced Wooden Sword"),
    description = vt_system.Translate("A sword made of wood with a steel hilt, reinforced magically."),
    icon = "data/inventory/weapons/woodensword.png",
    physical_attack = 30,
    magical_attack = 10,
    standard_price = 1200,
    usable_by = BRONANN + THANIS,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 1200,  -- price
        [3119] = 5,  -- Fine Wood
        [3001] = 3   -- 3 Copper ore
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua"
        }
    }
}

weapons[10004] = {
    name = vt_system.Translate("Perfect Wooden Sword"),
    description = vt_system.Translate("A sword made of wood with a steel hilt, surrounded by a great magical force."),
    icon = "data/inventory/weapons/woodensword.png",
    physical_attack = 60,
    magical_attack = 20,
    standard_price = 4500,
    usable_by = BRONANN + THANIS,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 4500,  -- price
        [10002] = 1, -- 1 Reinforced wooden sword
        [3001] = 5   -- 5 Copper ore
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/wood_bronze/bronann_hurt.lua"
        }
    }
}


-- Iron/fire family
-- -----------------
weapons[10011] = {
    name = vt_system.Translate("Iron Sword"),
    description = vt_system.Translate("A sturdy but somewhat dull sword whose blade was forged from a single block of solid iron."),
    icon = "data/inventory/weapons/iron_sword.png",
    physical_attack = 36,
    magical_attack = 0,
    standard_price = 250,
    usable_by = BRONANN + THANIS,
    slots = 0,

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/steel/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10012] = {
    name = vt_system.Translate("Soldier Sword"),
    description = vt_system.Translate("Standard soldier sword. A light weight iron sword suitable for most skirmishes."),
    icon = "data/inventory/weapons/karlate_sword.png",
    physical_attack = 50,
    magical_attack = 0,
    standard_price = 1250,
    usable_by = BRONANN + THANIS,
    slots = 0,

    trade_conditions = {
        [0] = 1250,  -- price
        [10011] = 1, -- 1 Iron sword
        [3001] = 3,  -- 3 Copper ore
        [3002] = 1   -- 1 Iron ore
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/steel/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10013] = {
    name = vt_system.Translate("Knight's Blade"),
    description = vt_system.Translate("A weapon bestowed to seasoned veterans of the knighthood."),
    icon = "data/inventory/weapons/knights_blade.png",
    physical_attack = 70,
    magical_attack = 0,
    standard_price = 2180,
    usable_by = BRONANN + THANIS,
    slots = 1,

    trade_conditions = {
        [0] = 1150,   -- price
        [10012] = 1, -- 1 Soldier sword
        [3002] = 3,  -- 3 Iron ore
        [3008] = 1   -- 1 Phoenix Feather
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/steel/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

weapons[10014] = {
    name = vt_system.Translate("Paladin's Sword"),
    description = vt_system.Translate("A mythical weapon blessed with a magical fire."),
    icon = "data/inventory/weapons/paladin-sword.png",
    physical_attack = 90,
    magical_attack = 30,
    standard_price = 4340,
    usable_by = BRONANN + THANIS,
    slots = 3,

    trade_conditions = {
        [0] = 4340,  -- price
        [10013] = 1, -- 1 Knight's blade
        [3002] = 5,   -- 5 Iron Ore
        [3008] = 3   -- 3 Phoenix Feather
        -- TODO: Add 1 fire salamander
    },

    status_effects = {
        [vt_global.GameGlobal.GLOBAL_STATUS_MAG_DEF] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
        [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER,
        [vt_global.GameGlobal.GLOBAL_STATUS_LIFE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER
    },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/steel/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

-- NOTE: Test weapon
weapons[10999] = {
    name = vt_system.Translate("Omni Sword"),
    description = "The ultimate sword, used only for testing...",
    icon = "data/inventory/weapons/sword-flaming.png",
    physical_attack = 9999,
    magical_attack = 9999,
    standard_price = 9999999,
    usable_by = BRONANN + THANIS,
    slots = 5,
    key_item = true,

    -- NOTE: Testing trade conditions,
    trade_conditions = {
        [0] = 20000,  -- price
        [1003] = 5,   -- 5 Elixirs
        [1] = 3,      -- 3 minor healing potions
        [2] = 2,      -- 2 medium healing potions
        [3] = 6,      -- 6 healing potions
        [4] = 5,      -- 5 Mega healing potions
        [11] = 8      -- 8 Small moon juice
    },

    status_effects = {
        [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_MAG_DEF] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_DEF] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_HP] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_SP] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_FIRE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_WATER] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_VOLT] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_EARTH] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_LIFE] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_DEATH] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME,
        [vt_global.GameGlobal.GLOBAL_STATUS_NEUTRAL] = vt_global.GameGlobal.GLOBAL_INTENSITY_POS_EXTREME
    },

    -- A max of 5 skills can be earned through a piece of equipment.
    equipment_skills = { 10100, 10119, 10101, 10163, 10164 },

    battle_animations = {
        [BRONANN] = {
            idle = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            run = "data/entities/battle/characters/bronann/weapons/steel/bronann_run.lua",
            run_after_victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_run_after_victory.lua",
            attack = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack.lua",
            attack_forward_thrust = "data/entities/battle/characters/bronann/weapons/steel/bronann_attack_forward_thrust.lua",
            dodge = "data/entities/battle/characters/bronann/weapons/steel/bronann_dodge.lua",
            victory = "data/entities/battle/characters/bronann/weapons/steel/bronann_victory.lua",
            hurt = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua",
            poor = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dying = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            dead = "data/entities/battle/characters/bronann/weapons/steel/bronann_dead.lua",
            revive = "data/entities/battle/characters/bronann/weapons/steel/bronann_kneeling.lua",
            item = "data/entities/battle/characters/bronann/weapons/steel/bronann_idle.lua",
            magic_prepare = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/bronann/weapons/steel/bronann_magic_cast.lua",
            jump_forward = "data/entities/battle/characters/bronann/weapons/steel/bronann_jump_forward.lua",
            jump_backward = "data/entities/battle/characters/bronann/weapons/steel/bronann_hurt.lua"
        }
    }
}

-- -----------------------------------------------------------------------------
-- IDs 11,001 - 11,500 are reserved for arbalests
-- -----------------------------------------------------------------------------

weapons[11001] = {
    name = vt_system.Translate("Arbalest"),
    description = vt_system.Translate("A standard wooden arbalest."),
    icon = "data/inventory/weapons/arbalest.png",
    -- The image displayed when kalya fires with her arbalest.
    battle_ammo_animation_file = "data/entities/battle/ammo/wood_arrow.lua",
    physical_attack = 3,
    magical_attack = 0,
    standard_price = 50,
    usable_by = KALYA,
    slots = 0,
    key_item = true,

    battle_animations = {
        [KALYA] = {
            idle = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            run = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            run_after_victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            attack = "data/entities/battle/characters/kalya/weapons/steel/kalya_attack.lua",
            dodge = "data/entities/battle/characters/kalya/weapons/steel/kalya_dodge.lua",
            victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_victory.lua",
            hurt = "data/entities/battle/characters/kalya/weapons/steel/kalya_hurt.lua",
            poor = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dying = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dead = "data/entities/battle/characters/kalya/weapons/steel/kalya_dead.lua",
            revive = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            item = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            magic_prepare = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_cast.lua"
        }
    }
}

weapons[11002] = {
    name = vt_system.Translate("Improved Arbalest"),
    description = vt_system.Translate("A standard wooden arbalest."),
    icon = "data/inventory/weapons/arbalest.png",
    -- The image displayed when kalya fires with her arbalest.
    battle_ammo_animation_file = "data/entities/battle/ammo/wood_arrow.lua",
    physical_attack = 18,
    magical_attack = 3,
    standard_price = 50,
    usable_by = KALYA,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 50,    -- Price
        [3119] = 3,  -- Fine Wood
        [3112] = 1,  -- Silk
        [3102] = 2,  -- Insect Material
        [3103] = 1   -- Refined Insect Material
    },

    battle_animations = {
        [KALYA] = {
            idle = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            run = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            run_after_victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            attack = "data/entities/battle/characters/kalya/weapons/steel/kalya_attack.lua",
            dodge = "data/entities/battle/characters/kalya/weapons/steel/kalya_dodge.lua",
            victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_victory.lua",
            hurt = "data/entities/battle/characters/kalya/weapons/steel/kalya_hurt.lua",
            poor = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dying = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dead = "data/entities/battle/characters/kalya/weapons/steel/kalya_dead.lua",
            revive = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            item = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            magic_prepare = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_cast.lua"
        }
    }
}

weapons[11003] = {
    name = vt_system.Translate("Noble Arbalest"),
    description = vt_system.Translate("An arbalest made out of fine noble ash tree wood."),
    icon = "data/inventory/weapons/arbalest.png",
    -- The image displayed when kalya fires with her arbalest.
    battle_ammo_animation_file = "data/entities/battle/ammo/wood_arrow.lua",
    physical_attack = 34,
    magical_attack = 12,
    standard_price = 1450,
    usable_by = KALYA,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 1450,  -- price
        [3119] = 3,  -- Fine Wood
        [3001] = 2,  -- Copper ore
        [3002] = 1   -- Iron ore
    },

    battle_animations = {
        [KALYA] = {
            idle = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            run = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            run_after_victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            attack = "data/entities/battle/characters/kalya/weapons/steel/kalya_attack.lua",
            dodge = "data/entities/battle/characters/kalya/weapons/steel/kalya_dodge.lua",
            victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_victory.lua",
            hurt = "data/entities/battle/characters/kalya/weapons/steel/kalya_hurt.lua",
            poor = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dying = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dead = "data/entities/battle/characters/kalya/weapons/steel/kalya_dead.lua",
            revive = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            item = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            magic_prepare = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_cast.lua"
        }
    }
}

weapons[11004] = {
    name = vt_system.Translate("Arbalest of Force"),
    description = vt_system.Translate("An arbalest strengthened with magical ore."),
    icon = "data/inventory/weapons/arbalest.png",
    -- The image displayed when kalya fires with her arbalest.
    battle_ammo_animation_file = "data/entities/battle/ammo/wood_arrow.lua",
    physical_attack = 25,
    magical_attack = 24,
    standard_price = 1650,
    usable_by = KALYA,
    slots = 0,
    key_item = true,

    trade_conditions = {
        [0] = 1650,  -- price
        [3119] = 10, -- Fine Wood
        [3001] = 1,  -- Copper ore
        [3002] = 2,  -- Iron ore
        [1003] = 1,  -- Elixir
    },

    battle_animations = {
        [KALYA] = {
            idle = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            run = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            run_after_victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_run.lua",
            attack = "data/entities/battle/characters/kalya/weapons/steel/kalya_attack.lua",
            dodge = "data/entities/battle/characters/kalya/weapons/steel/kalya_dodge.lua",
            victory = "data/entities/battle/characters/kalya/weapons/steel/kalya_victory.lua",
            hurt = "data/entities/battle/characters/kalya/weapons/steel/kalya_hurt.lua",
            poor = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dying = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            dead = "data/entities/battle/characters/kalya/weapons/steel/kalya_dead.lua",
            revive = "data/entities/battle/characters/kalya/weapons/steel/kalya_kneeling.lua",
            item = "data/entities/battle/characters/kalya/weapons/steel/kalya_idle.lua",
            magic_prepare = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_prepare.lua",
            magic_cast = "data/entities/battle/characters/kalya/weapons/steel/kalya_magic_cast.lua"
        }
    }
}
