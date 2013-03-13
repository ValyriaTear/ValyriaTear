------------------------------------------------------------------------------[[
-- Filename: enemies.lua
--
-- Description: This file contains the definitions of multiple foes that the
-- player encounters in battle. This file contains those enemies who have ids
-- from 1-100.
--
------------------------------------------------------------------------------]]

-- All enemy definitions are stored in this table
-- check to see if the enemies table has already been created by another script
enemies = {}


enemies[1] = {
    name = hoa_system.Translate("Green Slime"),
    stamina_icon = "img/icons/actors/enemies/green_slime.png",
    battle_sprites = "img/sprites/battle/enemies/green_slime.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 55,
        skill_points = 0,
        strength = 11,
        vigor = 0,
        fortitude = 4,
        protection = 1,
        agility = 20,
        evade = 2.0,
        experience_points = 15,
        drunes = 8
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Body"),
            x_position = 5,
            y_position = 34,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1001
    },

    drop_objects = {

    }
}

enemies[2] = {
    name = hoa_system.Translate("Spider"),
    stamina_icon = "img/icons/actors/enemies/spider.png",
    battle_sprites = "img/sprites/battle/enemies/spider.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 65,
        skill_points = 0,
        strength = 14,
        vigor = 0,
        fortitude = 5,
        protection = 4,
        agility = 25,
        evade = 2.0,
        experience_points = 19,
        drunes = 10
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -15,
            y_position = 33,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Abdomen"),
            x_position = 16,
            y_position = 57,
            fortitude_modifier = 0.2,
            protection_modifier = 0.0,
            evade_modifier = -0.2
        }
    },

    skills = {
        1002
    },

    drop_objects = {
        { 1, 0.10 } -- Minor Healing Potion
    }
}

-- Layna forest first boss - 1st encounter
enemies[3] = {
    name = hoa_system.Translate("Fenrir"),
    stamina_icon = "img/icons/actors/enemies/fenrir.png",
    battle_sprites = "img/sprites/battle/enemies/fenrir_normal.png",
    sprite_width = 268,
    sprite_height = 196,

    scripts = {
        death = "dat/battles/enemies_animations/fenrir_flee.lua"
    },

    base_stats = {
        hit_points = 160,
        skill_points = 10,
        strength = 20,
        vigor = 14,
        fortitude = 6,
        protection = 6,
        agility = 40,
        evade = 4.0,
        experience_points = 44,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            fortitude_modifier = -0.2,
            protection_modifier = 0,
            evade_modifier = 0.2
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            fortitude_modifier = 0.0,
            protection_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008
    },

    drop_objects = {
    { 1, 1.0 }, -- Minor Healing Potion x2
    { 1, 1.0 }
    }
}


enemies[4] = {
    name = hoa_system.Translate("Snake"),
    stamina_icon = "img/icons/actors/enemies/green_snake.png",
    battle_sprites = "img/sprites/battle/enemies/green_snake.png",
    sprite_width = 128,
    sprite_height = 64,

    base_stats = {
        hit_points = 80,
        skill_points = 10,
        strength = 22,
        vigor = 20,
        fortitude = 8,
        protection = 5,
        agility = 30,
        evade = 6.0,
        experience_points = 31,
        drunes = 15
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -40,
            y_position = 60,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = -6,
            y_position = 25,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Tail"),
            x_position = 14,
            y_position = 38,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
        }
    },

    skills = {
        1003, -- Normal attack
        1004, -- Stun bite
        1005  -- Dampening bite (Agility lowering attack)
    },

    drop_objects = {
        { 1, 0.10 }, -- Minor Healing Potion
        { 11, 0.10 }, -- Minor Moon Juice Potion
    }
}

enemies[5] = {
    name = hoa_system.Translate("Slime Mother"),
    stamina_icon = "img/icons/actors/enemies/big_slime.png",
    battle_sprites = "img/sprites/battle/enemies/big_slime.png",
    sprite_width = 128,
    sprite_height = 128,

    base_stats = {
        hit_points = 355,
        skill_points = 0,
        strength = 35,
        vigor = 10,
        fortitude = 10,
        protection = 10,
        agility = 18,
        evade = 2.0,
        experience_points = 89,
        drunes = 56
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Body"),
            x_position = 5,
            y_position = 34,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1001,
        21001
    },

    drop_objects = {

    }
}


enemies[6] = {
    name = hoa_system.Translate("Bat"),
    stamina_icon = "img/icons/actors/enemies/bat.png",
    battle_sprites = "img/sprites/battle/enemies/bat.png",
    sprite_width = 64,
    sprite_height = 128,

    base_stats = {
        hit_points = 75,
        skill_points = 10,
        strength = 22,
        vigor = 20,
        fortitude = 10,
        protection = 8,
        agility = 35,
        evade = 6.0,
        experience_points = 32,
        drunes = 15
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Torso"),
            x_position = 0,
            y_position = 95,
            fortitude_modifier = 0.2,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Wing"),
            x_position = -40,
            y_position = 90,
            fortitude_modifier = -0.5,
            protection_modifier = 0.0,
            evade_modifier = 0.4,
            status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 25.0 }
        },
    },

    skills = {
        1007  -- HP drain
    },

    drop_objects = {
        { 1, 0.15 },  -- Minor Healing Potion
        { 11, 0.15 } -- Minor Moon Juice
    }
}

-- Layna forest first boss - 2nd encounter
enemies[7] = {
    name = hoa_system.Translate("Fenrir"),
    stamina_icon = "img/icons/actors/enemies/fenrir.png",
    battle_sprites = "img/sprites/battle/enemies/fenrir_normal.png",
    sprite_width = 268,
    sprite_height = 196,

    scripts = {
        death = "dat/battles/enemies_animations/fenrir_flee.lua"
    },

    base_stats = {
        hit_points = 420,
        skill_points = 10,
        strength = 27,
        vigor = 14,
        fortitude = 10,
        protection = 6,
        agility = 40,
        evade = 4.0,
        experience_points = 98,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            fortitude_modifier = -0.2,
            protection_modifier = 0,
            evade_modifier = 0.2
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            fortitude_modifier = 0.0,
            protection_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008
    },

    drop_objects = {
    { 1, 1.0 }, -- Minor Healing Potion x2
    { 1, 1.0 }
    }
}


-- Layna forest first boss - Last encounter
enemies[8] = {
    name = hoa_system.Translate("Fenrir"),
    stamina_icon = "img/icons/actors/enemies/fenrir.png",
    battle_sprites = "img/sprites/battle/enemies/fenrir_final.png",
    sprite_width = 268,
    sprite_height = 196,

    scripts = {
        death = "dat/battles/enemies_animations/fenrir_death.lua"
    },

    base_stats = {
        hit_points = 640,
        skill_points = 10,
        strength = 27,
        vigor = 14,
        fortitude = 10,
        protection = 6,
        agility = 40,
        evade = 4.0,
        experience_points = 142,
        drunes = 100
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            fortitude_modifier = -0.2,
            protection_modifier = 0,
            evade_modifier = 0.2
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            fortitude_modifier = 0.0,
            protection_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008
    },

    drop_objects = {
    { 1, 1.0 }, -- Minor Healing Potion x2
    { 1, 1.0 }
    }
}

enemies[9] = {
    name = hoa_system.Translate("Dark Soldier"),
    stamina_icon = "img/icons/actors/enemies/dark_soldier.png",
    battle_sprites = "img/sprites/battle/enemies/dark_soldier.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        strength = 18,
        vigor = 0,
        fortitude = 12,
        protection = 4,
        agility = 14,
        evade = 2.0,
        -- TODO: Balance XP and Drunes
        experience_points = 8,
        drunes = 12
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -6,
            y_position = 23,
            fortitude_modifier = -0.2,
            protection_modifier = -0.2,
            evade_modifier = 0.2
        },
        [2] = {
            name = hoa_system.Translate("Abdomen"),
            x_position = 7,
            y_position = 26,
            fortitude_modifier = 0.4,
            protection_modifier = 0.3,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Leg"),
            x_position = 16,
            y_position = 14,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.1,
            status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
        }
    },

    skills = {
        1006
    },

    drop_objects = {
        { 1, 0.15 }, -- Minor Healing Potion
        { 11, 0.15 } -- Minor Moon Juice
    }
}

-- ======== Unbalanced ========

-- Traits -----------------------------------------------------------------------
-- HSP: low HP, low SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: high agi, med eva
-- XPD: med XP, low drunes
--------------------------------------------------------------------------------
enemies[59] = {
    name = hoa_system.Translate("Scorpion"),
    stamina_icon = "img/icons/actors/enemies/scorpion.png",
    battle_sprites = "img/sprites/battle/enemies/scorpion.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        strength = 18,
        vigor = 0,
        fortitude = 12,
        protection = 4,
        agility = 14,
        evade = 2.0,
        experience_points = 8,
        drunes = 12
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -6,
            y_position = 23,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Abdomen"),
            x_position = 7,
            y_position = 26,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Leg"),
            x_position = 16,
            y_position = 14,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
        }
    },

    skills = {
        1002
    },

    drop_objects = {
        { 1, 0.15 }, -- Minor Healing Potion
        { 11, 0.15 } -- Minor Moon Juice
    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: ??? HP, ??? SP
-- ATK: ??? phys, ??? meta
-- DEF: ??? phys, ??? meta
-- SPD: ??? agi, ??? eva
-- XPD: ??? XP, ??? drunes
--------------------------------------------------------------------------------
enemies[60] = {
    name = hoa_system.Translate("Dune Crawler"),
    stamina_icon = "img/icons/actors/enemies/dune_crawler.png",
    battle_sprites = "img/sprites/battle/enemies/dune_crawler.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        strength = 18,
        vigor = 0,
        fortitude = 12,
        protection = 4,
        agility = 14,
        evade = 2.0,
        experience_points = 8,
        drunes = 12
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -5,
            y_position = 22,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Tail"),
            x_position = 18,
            y_position = 45,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1002
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: med HP, med SP
-- ATK: high phys, zero meta
-- DEF: high phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, med drunes
--------------------------------------------------------------------------------
enemies[61] = {
    name = hoa_system.Translate("Skeleton"),
    stamina_icon = "img/icons/actors/enemies/skeleton.png",
    battle_sprites = "img/sprites/battle/enemies/skeleton.png",
    sprite_width = 64,
    sprite_height = 128,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -23,
            y_position = 108,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Chest"),
            x_position = -12,
            y_position = 82,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Leg"),
            x_position = -26,
            y_position = 56,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 20.0 }
        }
    },

    skills = {
        1006
    },

    drop_objects = {
        { 1, 0.15 } -- Minor Healing Potion
    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[62] = {
    name = hoa_system.Translate("Stygian Lizard"),
    stamina_icon = "img/icons/actors/enemies/stygian_lizard.png",
    battle_sprites = "img/sprites/battle/enemies/stygian_lizard.png",
    sprite_width = 192,
    sprite_height = 192,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Eye"),
            x_position = 10,
            y_position = 160,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Torso"),
            x_position = -13,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Claw"),
            x_position = -60,
            y_position = 115,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006, -- Normal attack
        10004 -- Fire burst
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: med HP, med SP
-- ATK: low phys, med meta
-- DEF: low phys, med meta
-- SPD: med agi, med eva
-- XPD: med XP, med drunes
--------------------------------------------------------------------------------
enemies[63] = {
    name = hoa_system.Translate("Demonic Essence"),
    stamina_icon = "img/icons/actors/enemies/demonic_essence.png",
    battle_sprites = "img/sprites/battle/enemies/demonic_essence.png",
    sprite_width = 128,
    sprite_height = 192,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Face"),
            x_position = 0,
            y_position = 166,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = -8,
            y_position = 114,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Claw"),
            x_position = -48,
            y_position = 108,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006, -- Normal attack
        10002 -- First Aid
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[64] = {
    name = hoa_system.Translate("Yeti"),
    stamina_icon = "img/icons/actors/enemies/yeti.png",
    battle_sprites = "img/sprites/battle/enemies/yeti.png",
    sprite_width = 128,
    sprite_height = 128,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Torso"),
            x_position = -13,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[65] = {
    name = hoa_system.Translate("Red Slime"),
    stamina_icon = "img/icons/actors/enemies/red_slime.png",
    battle_sprites = "img/sprites/battle/enemies/red_slime.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Body"),
            x_position = -13,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[66] = {
    name = hoa_system.Translate("Thing"),
    stamina_icon = "img/icons/actors/enemies/thing.png",
    battle_sprites = "img/sprites/battle/enemies/thing.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Body"),
            x_position = -13,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[67] = {
    name = hoa_system.Translate("Mushroom Zombie"),
    stamina_icon = "img/icons/actors/enemies/mushroom_zombie.png",
    battle_sprites = "img/sprites/battle/enemies/mushroom_zombie.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        strength = 15,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Body"),
            x_position = -13,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: med HP, low SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: med agi, med eva
-- XPD: low XP, med drunes
--------------------------------------------------------------------------------
enemies[68] = {
    name = hoa_system.Translate("Rat"),
    stamina_icon = "img/icons/actors/enemies/rat.png",
    battle_sprites = "img/sprites/battle/enemies/rat.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 90,
        skill_points = 5,
        strength = 12,
        vigor = 0,
        fortitude = 14,
        protection = 4,
        agility = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 18
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -24,
            y_position = 50,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Chest"),
            x_position = -8,
            y_position = 25,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {
        { 1, 0.15 } -- Minor Healing Potion
    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: vhigh HP, med SP
-- ATK: high phys, zero meta
-- DEF: high phys, low meta
-- SPD: med agi, low eva
-- XPD: vhigh XP, high drunes
-- Notes: First boss in prologue module
--------------------------------------------------------------------------------
enemies[91] = {
    name = hoa_system.Translate("Scorpion Goliath"),
    stamina_icon = "img/icons/actors/enemies/scorpion_goliath.png",
    battle_sprites = "img/sprites/battle/enemies/scorpion_goliath.png",
    sprite_width = 512,
    sprite_height = 448,

    base_stats = {
        hit_points = 500,
        skill_points = 45,
        strength = 55,
        vigor = 0,
        fortitude = 20,
        protection = 5,
        agility = 25,
        evade = 3.0,
        experience_points = 242,
        drunes = 135
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -30,
            y_position = 125,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Pincer"),
            x_position = -190,
            y_position = 120,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Leg"),
            x_position = 200,
            y_position = 160,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    skills = {
        1002 -- TEMP until specific boss skills available
    },

    drop_objects = {

    }
}

-- Traits -----------------------------------------------------------------------
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: med agi, low eva
-- XPD: high XP, high drunes
-- Notes: Second boss in prologue module
--------------------------------------------------------------------------------
enemies[92] = {
    name = hoa_system.Translate("Armored Beast"),
    stamina_icon = "img/icons/actors/enemies/armored_beast.png",
    battle_sprites = "img/sprites/battle/enemies/armored_beast.png",
    sprite_width = 256,
    sprite_height = 256,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        strength = 10,
        vigor = 0,
        fortitude = 10,
        protection = 4,
        agility = 30,
        evade = 18.0,
        experience_points = 8,
        drunes = 12
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Mesosoma"),
            x_position = -6,
            y_position = 23,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Pincers"),
            x_position = 7,
            y_position = 26,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Legs"),
            x_position = 7,
            y_position = 26,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[101] = {
    name = hoa_system.Translate("Daemarbora"),
    stamina_icon = "img/icons/actors/enemies/daemarbora.png",
    battle_sprites = "img/sprites/battle/enemies/daemarbora.png",
    sprite_width = 128,
    sprite_height = 128,

    initial_stats = {
        hit_points = 80,
        skill_points = 20,
        experience_points = 15,
        strength = 12,
        vigor = 20,
        fortitude = 15,
        protection = 7,
        agility = 8,
        evade = 1.0,
        drunes = 55
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Trunk"),
            x_position = -6,
            y_position = 36,
            fortitude_modifier = 0,
            protection_modifier = 0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Branches"),
            x_position = 0,
            y_position = 80,
            fortitude_modifier = 0,
            protection_modifier = 0,
            evade_modifier = 1.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[102] = {
    name = hoa_system.Translate("Aerocephal"),
    stamina_icon = "img/icons/actors/enemies/aerocephal.png",
    battle_sprites = "img/sprites/battle/enemies/aerocephal.png",
    sprite_width = 192,
    sprite_height = 192,

    initial_stats = {
        hit_points = 90,
        skill_points = 10,
        experience_points = 12,
        strength = 8,
        vigor = 0,
        fortitude = 7,
        protection = 4,
        agility = 20,
        evade = 10.0,
        drunes = 60
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Forehead"),
            x_position = -6,
            y_position = 127,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Orifice"),
            x_position = -1,
            y_position = 77,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[103] = {
    name = hoa_system.Translate("Arcana Drake"),
    stamina_icon = "img/icons/actors/enemies/arcana_drake.png",
    battle_sprites = "img/sprites/battle/enemies/arcana_drake.png",
    sprite_width = 192,
    sprite_height = 256,

    initial_stats = {
        hit_points = 85,
        skill_points = 10,
        experience_points = 45,
        strength = 20,
        vigor = 0,
        fortitude = 8,
        protection = 5,
        agility = 15,
        evade = 2.0,
        drunes = 80
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -6,
            y_position = 167,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Body"),
            x_position = -1,
            y_position = 111,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Tail"),
            x_position = -74,
            y_position = 146,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[104] = {
    name = hoa_system.Translate("Nagaruda"),
    stamina_icon = "img/icons/actors/enemies/nagaruda.png",
    battle_sprites = "img/sprites/battle/enemies/nagaruda.png",
    sprite_width = 192,
    sprite_height = 256,

    initial_stats = {
        hit_points = 90,
        skill_points = 10,
        experience_points = 18,
        strength = 10,
        vigor = 0,
        fortitude = 8,
        protection = 4,
        agility = 12,
        evade = 2.0,
        drunes = 70
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -26,
            y_position = 165,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Abdomen"),
            x_position = -36,
            y_position = 115,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Tail"),
            x_position = -26,
            y_position = 65,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[105] = {
    name = hoa_system.Translate("Deceleon"),
    stamina_icon = "img/icons/actors/enemies/deceleon.png",
    battle_sprites = "img/sprites/battle/enemies/deceleon.png",
    sprite_width = 256,
    sprite_height = 256,

    initial_stats = {
        hit_points = 100,
        skill_points = 10,
        experience_points = 18,
        strength = 15,
        vigor = 0,
        fortitude = 20,
        protection = 4,
        agility = 4,
        evade = 1.0,
        drunes = 85
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -104,
            y_position = 226,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Chest"),
            x_position = -106,
            y_position = 190,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Arm"),
            x_position = -56,
            y_position = 155,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [4] = {
            name = hoa_system.Translate("Legs"),
            x_position = -106,
            y_position = 105,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[106] = {
    name = hoa_system.Translate("Aurum Drakueli"),
    stamina_icon = "img/icons/actors/enemies/aurum_drakueli.png",
    battle_sprites = "img/sprites/battle/enemies/aurum_drakueli.png",
    sprite_width = 320,
    sprite_height = 256,


    initial_stats = {
        hit_points = 120,
        skill_points = 10,
        experience_points = 20,
        strength = 22,
        vigor = 0,
        fortitude = 8,
        protection = 4,
        agility = 18,
        evade = 2.0,
        drunes = 100
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = -4,
            y_position = 222,
            fortitude_modifier = 0,
            protection_modifier = 0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Chest"),
            x_position = 39,
            y_position = 155,
            fortitude_modifier = 0,
            protection_modifier = 0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Arm"),
            x_position = 82,
            y_position = 143,
            fortitude_modifier = 0,
            protection_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}

enemies[107] = {
    name = hoa_system.Translate("Kyle"),
    stamina_icon = "img/icons/actors/enemies/kyle.png",
    battle_sprites = "img/sprites/battle/enemies/kyle.png",
    sprite_width = 48,
    sprite_height = 96,

    growth_stats = {
        hit_points = 5.0,
        skill_points = 1.0,
        experience_points = 1.5,
        strength = 5.0,
        vigor = 1.0,
        fortitude = 3.0,
        protection = 1.0,
        agility = 1.0,
        evade = 0.0,
        drunes = 2.0
    },

    attack_points = {
        [1] = {
            name = hoa_system.Translate("Head"),
            x_position = 21,
            y_position = 80,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = hoa_system.Translate("Chest"),
            x_position = 21,
            y_position = 46,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = hoa_system.Translate("Legs"),
            x_position = 21,
            y_position = 14,
            fortitude_modifier = 0.0,
            protection_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}
