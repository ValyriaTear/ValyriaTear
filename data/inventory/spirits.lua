-- Spirits
-- Elemental spirits that can possess a weapon to improve it.

-- All spirits definitions are stored in this table
if (spirits == nil) then
   spirits = {}
end


-- -----------------------------------------------------------------------------
-- IDs 60,001 - 70,000 are reserved for spirits
-- -----------------------------------------------------------------------------

local ARDORIS  = 60001;
local UNDINE   = 60002;
local SYLPHID  = 60003;
local DRYAD    = 60004;
local FAIRY    = 60005;
local DEMONA   = 60006;
local OBLIVION = 60007;

spirits[ARDORIS] = {
    name = vt_system.Translate("Ardoris"),
    description = vt_system.Translate("An enchanting child of fire."),
    icon = "data/inventory/items/ardoris.png",
    standard_price = 4500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_FIRE, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_WATER, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10100, -- Fire burst
            [UNDINE]   = 10162, -- ? (Antipode)
            [SYLPHID]  = 10104, -- Fire Breath
            [DRYAD]    = 10102, -- Lava
            [FAIRY]    = 10106, -- Holy Fire
            [DEMONA]   = 10108, -- Petrify
            [OBLIVION] = 10163  -- Counter
        },
        defense = {
            [ARDORIS]  = 10101, -- Fire circle
            [UNDINE]   = 10103, -- Fog
            [SYLPHID]  = 10105, -- Focus
            [DRYAD]    = 10001, -- Shield
            [FAIRY]    = 10107, -- Life
            [DEMONA]   = 10109, -- Rage
            [OBLIVION] = 10164  -- Phoenix Call
        }
    }
}

spirits[UNDINE] = {
    name = vt_system.Translate("Undine"),
    description = vt_system.Translate("A mesmerizing spirit of living water."),
    icon = "data/inventory/items/undine.png",
    standard_price = 4500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_WATER, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_FIRE, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10162, -- ? (Antipode)
            [UNDINE]   = 10120, -- Wave
            [SYLPHID]  = 10122, -- Whirlwind
            [DRYAD]    = 10112, -- Entangle
            [FAIRY]    = 10124, -- Blindness
            [DEMONA]   = 10126, -- Poison
            [OBLIVION] = 10128  -- Tsunami
        },
        defense = {
            [ARDORIS]  = 10103, -- Fog
            [UNDINE]   = 10121, -- Water circle
            [SYLPHID]  = 10123, -- Slow
            [DRYAD]    = 10113, -- Magical Armor
            [FAIRY]    = 10125, -- Heal
            [DEMONA]   = 10127, -- Stun
            [OBLIVION] = 10129  -- Heal Rain
        }
    }
}

spirits[SYLPHID] = {
    name = vt_system.Translate("Sylphid"),
    description = vt_system.Translate("A very rare spirit of wind, usually neither tamable nor catchable."),
    icon = "data/inventory/items/sylphid.png",
    standard_price = 4500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_VOLT, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_EARTH, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10104, -- Fire Breath
            [UNDINE]   = 10122, -- Whirlwind
            [SYLPHID]  = 10130, -- Wind
            [DRYAD]    = 10162, -- ? (antipode)
            [FAIRY]    = 10132, -- Lightning
            [DEMONA]   = 10134, -- Magic Drain
            [OBLIVION] = 10136  -- Storm
        },
        defense = {
            [ARDORIS]  = 10105, -- Focus
            [UNDINE]   = 10123, -- Slow
            [SYLPHID]  = 10131, -- Wind Circle
            [DRYAD]    = 10114, -- Haste
            [FAIRY]    = 10133, -- Regen
            [DEMONA]   = 10135, -- Silence
            [OBLIVION] = 10137  -- Stop
        }
    }
}

spirits[DRYAD] = {
    name = vt_system.Translate("Dryad"),
    description = vt_system.Translate("A fragile spirit, child of mother nature."),
    icon = "data/inventory/items/dryad.png",
    standard_price = 4500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_EARTH, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_VOLT, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10102, -- Lava
            [UNDINE]   = 10112, -- Entangle
            [SYLPHID]  = 10162, -- ? (antipode)
            [DRYAD]    = 10110, -- Rock fall
            [FAIRY]    = 10115, -- Moon
            [DEMONA]   = 10117, -- Drain
            [OBLIVION] = 10119  -- Meteor
        },
        defense = {
            [ARDORIS]  = 10001, -- Shield
            [UNDINE]   = 10113, -- Magical Armor
            [SYLPHID]  = 10114, -- Haste
            [DRYAD]    = 10111, -- Earth Circle
            [FAIRY]    = 10116, -- Cure
            [DEMONA]   = 10118, -- Curse
            [OBLIVION] = 10165  -- Bio
        }
    }
}

spirits[FAIRY] = {
    name = vt_system.Translate("Fairy"),
    description = vt_system.Translate("Blinding as the sun, she brings life even where death belongs."),
    icon = "data/inventory/items/fairy.png",
    standard_price = 5500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_LIFE, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_DEATH, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10106, -- Holy Fire
            [UNDINE]   = 10124, -- Blindness
            [SYLPHID]  = 10132, -- Lightning
            [DRYAD]    = 10115, -- Moon
            [FAIRY]    = 10140, -- Holy
            [DEMONA]   = 10162, -- ? (antipode)
            [OBLIVION] = 10143  -- Sun Haze
        },
        defense = {
            [ARDORIS]  = 10107, -- Life
            [UNDINE]   = 10125, -- Heal
            [SYLPHID]  = 10133, -- Regen
            [DRYAD]    = 10116, -- Cure
            [FAIRY]    = 10141, -- Life Circle
            [DEMONA]   = 10142, -- Purify
            [OBLIVION] = 10144  -- Aura
        }
    }
}

spirits[DEMONA] = {
    name = vt_system.Translate("Demona"),
    description = vt_system.Translate("Spirit of the dark. It can't be possessed: it only chose to be possessed, waiting for its hour."),
    icon = "data/inventory/items/demona.png",
    standard_price = 5500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_DEATH, intensity = GLOBAL_INTENSITY_POS_LESSER },
        [1] = { element = GLOBAL_ELEMENTAL_LIFE, intensity = GLOBAL_INTENSITY_NEG_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10108, -- Petrify
            [UNDINE]   = 10126, -- Poison
            [SYLPHID]  = 10134, -- Magic Drain
            [DRYAD]    = 10117, -- Drain
            [FAIRY]    = 10162, -- ? (antipode)
            [DEMONA]   = 10150, -- Death
            [OBLIVION] = 10152  -- Doom
        },
        defense = {
            [ARDORIS]  = 10109, -- Rage
            [UNDINE]   = 10127, -- Stun
            [SYLPHID]  = 10135, -- Silence
            [DRYAD]    = 10118, -- Curse
            [FAIRY]    = 10142, -- Purify
            [DEMONA]   = 10151, -- Death Circle
            [OBLIVION] = 10153  -- Magical Reflection
        }
    }
}

spirits[OBLIVION] = {
    name = vt_system.Translate("Oblivion"),
    description = vt_system.Translate("Touched neither by time nor space, this spirit is dwelling in pure energy."),
    icon = "data/inventory/items/oblivion.png",
    standard_price = 7500,
    effects = {
        [0] = { element = GLOBAL_ELEMENTAL_NEUTRAL, intensity = GLOBAL_INTENSITY_POS_LESSER }
    },
    combinations = {
        -- [Combined with] = skill id obtained
        attack = {
            [ARDORIS]  = 10163, -- Counter
            [UNDINE]   = 10128, -- Tsunami
            [SYLPHID]  = 10136, -- Storm
            [DRYAD]    = 10119, -- Meteor
            [FAIRY]    = 10143, -- Sun Haze
            [DEMONA]   = 10152, -- Doom
            [OBLIVION] = 10160  -- Dispell
        },
        defense = {
            [ARDORIS]  = 10164, -- Phoenix Call
            [UNDINE]   = 10129, -- Heal Rain
            [SYLPHID]  = 10137, -- Stop
            [DRYAD]    = 10165, -- Bio
            [FAIRY]    = 10144, -- Aura
            [DEMONA]   = 10153, -- Magical Reflection
            [OBLIVION] = 10153  -- Shell
        }
    }
}
