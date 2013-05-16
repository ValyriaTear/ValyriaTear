-- Spirits
-- Elemental spirits that can possess a weapon to improve it.

-- All spiritq definitions are stored in this table
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
    description = vt_system.Translate("An enchanting child of Fire."),
    icon = "img/icons/items/ardoris.png",
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
        defence = {
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
    icon = "img/icons/items/undine.png",
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
        defence = {
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
    description = vt_system.Translate("A very rare spirit of Wind, usually neither tamable nor catchable."),
    icon = "img/icons/items/sylphid.png",
    standard_price = 4500
}

spirits[DRYAD] = {
    name = vt_system.Translate("Dryad"),
    description = vt_system.Translate("A fragile spirit, child of Mother Nature."),
    icon = "img/icons/items/dryad.png",
    standard_price = 4500
}

spirits[FAIRY] = {
    name = vt_system.Translate("Fairy"),
    description = vt_system.Translate("Blinding as the sun, she brings life even where Death belongs."),
    icon = "img/icons/items/fairy.png",
    standard_price = 5500
}

spirits[DEMONA] = {
    name = vt_system.Translate("Demona"),
    description = vt_system.Translate("Spirit of the Dark. It can't be possessed: it only chose to be possessed, waiting for its hour..."),
    icon = "img/icons/items/demona.png",
    standard_price = 5500
}

spirits[OBLIVION] = {
    name = vt_system.Translate("Oblivion"),
    description = vt_system.Translate("Touched neither by the time nor space, this spirit is dwelling in pure energy."),
    icon = "img/icons/items/oblivion.png",
    standard_price = 7500
}
