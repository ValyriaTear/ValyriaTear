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

-- Enemy hurt levels
local HURT_NONE = vt_global.GameGlobal.GLOBAL_ENEMY_HURT_NONE;
local HURT_SLIGHTLY = vt_global.GameGlobal.GLOBAL_ENEMY_HURT_SLIGHTLY;
local HURT_MEDIUM = vt_global.GameGlobal.GLOBAL_ENEMY_HURT_MEDIUM;
local HURT_HEAVILY = vt_global.GameGlobal.GLOBAL_ENEMY_HURT_HEAVILY;


enemies[1] = {
    name = vt_system.Translate("Green Slime"),
    stamina_icon = "data/battles/stamina_icons/enemies/green_slime.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/green_slime_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/green_slime_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/green_slime_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/green_slime_3.lua"
    },

    base_stats = {
        hit_points = 30,
        skill_points = 0,
        phys_atk = 11,
        mag_atk = 0,
        phys_def = 4,
        mag_def = 1,
        stamina = 20,
        evade = 2.0,
        experience_points = 15,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = 5,
            y_position = 34,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1001, -- standard bite
        1015  -- waterspray
    },

    drop_objects = {
        { 3100, 0.40 } -- Slimy Material
    }
}

enemies[2] = {
    name = vt_system.Translate("Spider"),
    stamina_icon = "data/battles/stamina_icons/enemies/spider.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/spider_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/spider_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/spider_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/spider_3.lua"
    },

    base_stats = {
        hit_points = 55,
        skill_points = 0,
        phys_atk = 14,
        mag_atk = 0,
        phys_def = 5,
        mag_def = 4,
        stamina = 25,
        evade = 2.0,
        experience_points = 19,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -15,
            y_position = 33,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Abdomen"),
            x_position = 16,
            y_position = 57,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = -0.2
        }
    },

    skills = {
        1002, -- standard attack
        1016, -- spider web
    },

    drop_objects = {
        { 3102, 0.40 }, -- Insect Material
        { 3112, 0.20 }, -- Silk
    }
}

-- Layna forest first boss - 1st encounter
enemies[3] = {
    name = vt_system.Translate("Fenrir"),
    stamina_icon = "data/battles/stamina_icons/enemies/fenrir.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/fenrir_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/fenrir_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/fenrir_1.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/fenrir_1.lua"
    },

    scripts = {
        death = "data/battles/enemies_animations/fenrir/fenrir_flee.lua"
    },

    base_stats = {
        hit_points = 160,
        skill_points = 10,
        phys_atk = 20,
        mag_atk = 14,
        phys_def = 6,
        mag_def = 6,
        stamina = 40,
        evade = 4.0,
        experience_points = 44,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            phys_def_modifier = -0.2,
            mag_def_modifier = 0,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008
    },

    drop_objects = {
        { 3104, 1.0 }, -- Fur
        { 3116, 1.0 }, -- Orb
    }
}


enemies[4] = {
    name = vt_system.Translate("Snake"),
    stamina_icon = "data/battles/stamina_icons/enemies/green_snake.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/green_snake_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/green_snake_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/green_snake_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/green_snake_3.lua"
    },

    base_stats = {
        hit_points = 80,
        skill_points = 10,
        phys_atk = 22,
        mag_atk = 20,
        phys_def = 8,
        mag_def = 5,
        stamina = 30,
        evade = 6.0,
        experience_points = 31,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -40,
            y_position = 60,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = -6,
            y_position = 25,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
        [3] = {
            name = vt_system.Translate("Tail"),
            x_position = 14,
            y_position = 38,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = 10.0 }
        }
    },

    skills = {
        1003, -- Normal attack
        1004, -- Stun bite
        1005  -- Dampening bite (Stamina lowering attack)
    },

    drop_objects = {
        { 3107, 0.40 }, -- Saurian Material
        { 3111, 0.20 }, -- Soft Powder
    }
}

enemies[5] = {
    name = vt_system.Translate("Slime Mother"),
    stamina_icon = "data/battles/stamina_icons/enemies/big_slime.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/big_slime_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/big_slime_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/big_slime_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/big_slime_3.lua"
    },

    base_stats = {
        hit_points = 355,
        skill_points = 0,
        phys_atk = 35,
        mag_atk = 10,
        phys_def = 10,
        mag_def = 10,
        stamina = 18,
        evade = 2.0,
        experience_points = 89,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = 5,
            y_position = 34,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        21001
    },

    drop_objects = {
        { 3101, 1.0 }, -- Refined Slimy Material
        { 3116, 1.0 }, -- Orb
    }
}


enemies[6] = {
    name = vt_system.Translate("Bat"),
    stamina_icon = "data/battles/stamina_icons/enemies/bat.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/bat_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/bat_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/bat_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/bat_3.lua"
    },

    base_stats = {
        hit_points = 75,
        skill_points = 10,
        phys_atk = 22,
        mag_atk = 20,
        phys_def = 10,
        mag_def = 8,
        stamina = 35,
        evade = 6.0,
        experience_points = 32,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Torso"),
            x_position = 0,
            y_position = 95,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
        [2] = {
            name = vt_system.Translate("Wing"),
            x_position = -40,
            y_position = 90,
            phys_def_modifier = -0.5,
            mag_def_modifier = 0.0,
            evade_modifier = 0.4,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 25.0 }
        },
    },

    skills = {
        1007  -- HP drain
    },

    drop_objects = {
        { 3109, 0.40 }, -- Rodent Material
        { 3117, 0.10 }, -- Spike
    }
}

-- Layna forest first boss - 2nd encounter
enemies[7] = {
    name = vt_system.Translate("Fenrir"),
    stamina_icon = "data/battles/stamina_icons/enemies/fenrir.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/fenrir_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/fenrir_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/fenrir_1.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/fenrir_1.lua"
    },

    scripts = {
        death = "data/battles/enemies_animations/fenrir/fenrir_flee.lua"
    },

    base_stats = {
        hit_points = 420,
        skill_points = 10,
        phys_atk = 27,
        mag_atk = 14,
        phys_def = 10,
        mag_def = 6,
        stamina = 40,
        evade = 4.0,
        experience_points = 98,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            phys_def_modifier = -0.2,
            mag_def_modifier = 0,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008
    },

    drop_objects = {
        { 1, 0.20 }, -- Minor Healing Potion x2
        { 1, 0.20 },
        { 3104, 1.0 } -- Fur
    }
}


-- Layna forest first boss - Last encounter
enemies[8] = {
    name = vt_system.Translate("Fenrir"),
    stamina_icon = "data/battles/stamina_icons/enemies/fenrir.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/fenrir_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/fenrir_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/fenrir_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/fenrir_3.lua"
    },

    scripts = {
        death = "data/battles/enemies_animations/fenrir/fenrir_death.lua"
    },

    base_stats = {
        hit_points = 640,
        skill_points = 10,
        phys_atk = 27,
        mag_atk = 14,
        phys_def = 10,
        mag_def = 20,
        stamina = 40,
        evade = 4.0,
        experience_points = 143,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -108,
            y_position = 156,
            phys_def_modifier = -0.2,
            mag_def_modifier = 0,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = 0,
            y_position = 120,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1008,
        1009
    },

    drop_objects = {
        { 3105, 1.0 } -- Soft Fur
    }
}

enemies[9] = {
    name = vt_system.Translate("Dark Soldier"),
    stamina_icon = "data/battles/stamina_icons/enemies/dark_soldier.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/dark_soldier_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/dark_soldier_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/dark_soldier_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/dark_soldier_0.lua"
    },

    base_stats = {
        hit_points = 300,
        skill_points = 30,
        phys_atk = 60,
        mag_atk = 28,
        phys_def = 60,
        mag_def = 35,
        stamina = 38,
        evade = 5.0,
        experience_points = 89,
        drunes = 58
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = 0,
            y_position = 280,
            phys_def_modifier = -0.2,
            mag_def_modifier = -0.2,
            evade_modifier = 0.2,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Torso"),
            x_position = 0,
            y_position = 200,
            phys_def_modifier = 0.4,
            mag_def_modifier = 0.3,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Legs"),
            x_position = -20,
            y_position = 100,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
        }
    },

    skills = {
        1006
    },

    drop_objects = {
        { 11, 0.35 }, -- Tiny Moon Juice
        { 4001, 0.15 } -- Escape Smoke
    }
}

enemies[10] = {
    name = vt_system.Translate("Lord Banesore"),
    stamina_icon = "data/battles/stamina_icons/enemies/lord_banesore.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/lord_banesore_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/lord_banesore_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/lord_banesore_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/lord_banesore_0.lua"
    },

    scripts = {
        battle_ai = "data/battles/enemies_ai/banesore_layna_village_ai.lua",
    },

    -- Special, unwinnable battle with no reward
    base_stats = {
        hit_points = 999999,
        skill_points = 99999,
        phys_atk = 40, -- Just for the river bank battle
        mag_atk = 40, -- just for the riverbank battle
        phys_def = 999,
        mag_def = 999,
        stamina = 27,
        evade = 8.0,
        experience_points = 0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = 20,
            y_position = 300,
            phys_def_modifier = -0.2,
            mag_def_modifier = -0.2,
            evade_modifier = 0.4,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_EVADE] = 10.0 }
        },
        [2] = {
            name = vt_system.Translate("Torso"),
            x_position = 20,
            y_position = 200,
            phys_def_modifier = 0.4,
            mag_def_modifier = 0.3,
            evade_modifier = 0.4
        },
        [3] = {
            name = vt_system.Translate("Legs"),
            x_position = 40,
            y_position = 100,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.4,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
        }
    },

    skills = {
        1006,
        10001,
        10100,
        10120
    },

    drop_objects = {
    }
}

enemies[11] = {
    name = vt_system.Translate("Shroom"),
    stamina_icon = "data/battles/stamina_icons/enemies/shroom.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/shroom_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/shroom_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/shroom_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/shroom_0.lua"
    },

    base_stats = {
        hit_points = 75,
        skill_points = 10,
        phys_atk = 22,
        mag_atk = 20,
        phys_def = 10,
        mag_def = 8,
        stamina = 35,
        evade = 6.0,
        experience_points = 32,
        drunes = 2
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = 0,
            y_position = 95,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
    },

    skills = {
        1007  -- HP drain
    },

    drop_objects = {
        { 3100, 0.30 }, -- Slimy Material
        { 3101, 0.10 }  -- Refined Slimy Material
    }
}

enemies[12] = {
    name = vt_system.Translate("Eyeball"),
    stamina_icon = "data/battles/stamina_icons/enemies/eyeball.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/eyeball_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/eyeball_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/eyeball_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/eyeball_0.lua"
    },

    base_stats = {
        hit_points = 75,
        skill_points = 10,
        phys_atk = 25,
        mag_atk = 20,
        phys_def = 15,
        mag_def = 8,
        stamina = 35,
        evade = 5.0,
        experience_points = 30,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Eye"),
            x_position = 0,
            y_position = 45,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
    },

    skills = {
        1007  -- HP drain
    },

    drop_objects = {
        { 3116, 0.30 }, -- Orb
    }
}

enemies[13] = {
    name = vt_system.Translate("Harlequin?"),
    stamina_icon = "data/battles/stamina_icons/enemies/harlequin.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/harlequin_0.lua"
    },

    base_stats = {
        hit_points = 150,
        skill_points = 150,
        phys_atk = 42,
        mag_atk = 35,
        phys_def = 25,
        mag_def = 25,
        stamina = 30,
        evade = 5.0,
        experience_points = 55,
        drunes = 30
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Torso"),
            x_position = 0,
            y_position = 95,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
    },

    skills = {
        21003,  -- HP/MP Drain
        10007, -- Magical poison
    },

    drop_objects = {
        { 1, 0.25 }, -- Tiny Healing Potion
        { 15, 0.05 } -- Lotus Petal (Cures poison)
    }
}

-- Harlequin first shape - but a little angrier
enemies[14] = {
    name = vt_system.Translate("Harlequin"),
    stamina_icon = "data/battles/stamina_icons/enemies/harlequin.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/harlequin_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/harlequin_0.lua"
    },

    base_stats = {
        hit_points = 300,
        skill_points = 450,
        phys_atk = 46,
        mag_atk = 45,
        phys_def = 30,
        mag_def = 30,
        stamina = 35,
        evade = 5.0,
        experience_points = 90,
        drunes = 300
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Torso"),
            x_position = 0,
            y_position = 95,
            phys_def_modifier = 0.2,
            mag_def_modifier = 0.0,
            evade_modifier = 0.1
        },
    },

    scripts = {
        battle_ai = "data/battles/enemies_ai/harlequin_ai.lua",
    },

    skills = {
        21003, -- HP/MP Drain
        10004, -- Holy Veil - ATK+M.ATK on all
        10011, -- War God - DEF + DEF-M on all
        10007, -- Magical poison
        21002, -- Dark Wish (Revives an ally)
     },

    drop_objects = {
        { 3002, 1.0 }, -- Iron Ore
        { 1005, 1.0 }  -- Haste potion
    }
}

enemies[15] = {
    name = vt_system.Translate("Beetle"),
    stamina_icon = "data/battles/stamina_icons/enemies/beetle.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/beetle_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/beetle_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/beetle_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/beetle_0.lua"
    },

    base_stats = {
        hit_points = 100,
        skill_points = 36,
        phys_atk = 30,
        mag_atk = 25,
        phys_def = 40,
        mag_def = 10,
        stamina = 35,
        evade = 5.0,
        experience_points = 45,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Chest"),
            x_position = -8,
            y_position = 25,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006, -- Normal atack
        1012  -- Stamina stealing attack
    },

    drop_objects = {
        { 3102, 0.40 }, -- Insect Material
        { 3103, 0.10 } -- Refined Insect Material
    }
}

enemies[16] = {
    name = vt_system.Translate("Rat"),
    stamina_icon = "data/battles/stamina_icons/enemies/rat.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/rat_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/rat_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/rat_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/rat_3.lua"
    },

    base_stats = {
        hit_points = 120,
        skill_points = 80,
        phys_atk = 35,
        mag_atk = 20,
        phys_def = 35,
        mag_def = 20,
        stamina = 40,
        evade = 7.0,
        experience_points = 47,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -28,
            y_position = 90,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Chest"),
            x_position = -8,
            y_position = 50,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006, -- Normal attack
        1010, -- Poison attack (5 SP)
    },

    drop_objects = {
        { 15, 0.06 },  -- Lotus petal - (Cure poison)
        { 3109, 0.40 } -- Rodent Material
    }
}

enemies[17] = {
    name = vt_system.Translate("Thing"),
    stamina_icon = "data/battles/stamina_icons/enemies/thing.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/thing_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/thing_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/thing_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/thing_0.lua"
    },

    base_stats = {
        hit_points = 124,
        skill_points = 50,
        phys_atk = 25,
        mag_atk = 30,
        phys_def = 40,
        mag_def = 20,
        stamina = 37,
        evade = 5.0,
        experience_points = 48,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = 0,
            y_position = 40,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006,  -- Normal attack
        10100, -- Fire (7 SP)
    },

    drop_objects = {
        { 1006, 0.04 }, -- Poison potion
        { 3117, 0.40 }  -- Stinging Material
    }
}

-- unbalanced
enemies[18] = {
    name = vt_system.Translate("Yeti"),
    stamina_icon = "data/battles/stamina_icons/enemies/yeti.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/yeti_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/yeti_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/yeti_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/yeti_0.lua"
    },

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 14,
        mag_def = 4,
        stamina = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Torso"),
            x_position = -13,
            y_position = 80,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006
    },

    drop_objects = {
       { 3105, 0.4 } -- Soft Fur
    }
}

enemies[19] = {
    name = vt_system.Translate("Skeleton"),
    stamina_icon = "data/battles/stamina_icons/enemies/skeleton.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/skeleton_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/skeleton_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/skeleton_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/skeleton_3.lua"
    },

    base_stats = {
        hit_points = 150,
        skill_points = 100,
        phys_atk = 30,
        mag_atk = 25,
        phys_def = 30,
        mag_def = 25,
        stamina = 35,
        evade = 5.0,
        experience_points = 50,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -23,
            y_position = 108,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Chest"),
            x_position = -12,
            y_position = 82,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Leg"),
            x_position = -26,
            y_position = 56,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 20.0 }
        }
    },

    skills = {
        1006, -- Normal attack
        1011, -- Frenzy attack (all characters)
    },

    drop_objects = {
        { 3117, 0.40 }, -- Stinging Material
    }
}

enemies[20] = {
    name = vt_system.Translate("Dorver"),
    stamina_icon = "data/battles/stamina_icons/enemies/dorver.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/dorver_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/dorver_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/dorver_0.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/dorver_0.lua"
    },

    base_stats = {
        hit_points = 300,
        skill_points = 100,
        phys_atk = 40,
        mag_atk = 20,
        phys_def = 50,
        mag_def = 20,
        stamina = 25,
        evade = 4.0,
        experience_points = 91,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -10,
            y_position = 26,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Chest"),
            x_position = 50,
            y_position = 40,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Leg"),
            x_position = 80,
            y_position = 86,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 20.0 }
        }
    },

    scripts = {
        battle_ai = "data/battles/enemies_ai/dorver_ai.lua",
    },

    skills = {
        1008, -- Attack
        1009, -- Attack all
        1013, -- Frenzy (atk & stamina +, but def -)
    },

    drop_objects = {
        { 3113, 1.0 }, -- Red Meat
        { 3113, 0.20 }
    }
}

enemies[21] = {
    name = vt_system.Translate("Andromalius"),
    stamina_icon = "data/battles/stamina_icons/enemies/andromalius.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/andromalius_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/andromalius_0.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/andromalius_1.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/andromalius_1.lua"
    },

    base_stats = {
        hit_points = 600,
        skill_points = 200,
        phys_atk = 30,
        mag_atk = 40,
        phys_def = 30,
        mag_def = 40,
        stamina = 40,
        evade = 6.0,
        experience_points = 200,
        drunes = 150
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -10,
            y_position = 26,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    scripts = {
        battle_ai = "data/battles/enemies_ai/andromalius_ai.lua",
    },

    skills = {
        1014,  -- Shake atk + AGI-
        10100, -- Fire
        10007, -- Magical poison
        21002, -- Dark Wish
    },

    drop_objects = {
        { 3002, 1.0 }, -- Iron ore
        { 1004, 0.04 } -- Periwinkle Potion (Physical Attack)
    }
}

enemies[22] = {
    name = vt_system.Translate("Ratto"),
    stamina_icon = "data/battles/stamina_icons/enemies/rat.png",

    battle_animations = {
        [HURT_NONE] = "data/entities/battle/enemies/rat_0.lua",
        [HURT_SLIGHTLY] = "data/entities/battle/enemies/rat_1.lua",
        [HURT_MEDIUM] = "data/entities/battle/enemies/rat_2.lua",
        [HURT_HEAVILY] = "data/entities/battle/enemies/rat_3.lua"
    },

    base_stats = {
        hit_points = 19,
        skill_points = 0,
        phys_atk = 6,
        mag_atk = 0,
        phys_def = 4,
        mag_def = 1,
        stamina = 20,
        evade = 2.0,
        experience_points = 6,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = 5,
            y_position = 34,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {
        1006, -- Normal attack
    },

    drop_objects = {
        { 3109, 0.20 } -- Rodent Material
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
    name = vt_system.Translate("Scorpion"),
    stamina_icon = "data/battles/stamina_icons/enemies/scorpion.png",

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        phys_atk = 18,
        mag_atk = 0,
        phys_def = 12,
        mag_def = 4,
        stamina = 14,
        evade = 2.0,
        experience_points = 8,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -6,
            y_position = 23,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Abdomen"),
            x_position = 7,
            y_position = 26,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Leg"),
            x_position = 16,
            y_position = 14,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
        }
    },

    skills = {
        1002
    },

    drop_objects = {
    }
}

enemies[60] = {
    name = vt_system.Translate("Dune Crawler"),
    stamina_icon = "data/battles/stamina_icons/enemies/dune_crawler.png",
    battle_sprites = "data/entities/battle/enemies/dune_crawler.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        phys_atk = 18,
        mag_atk = 0,
        phys_def = 12,
        mag_def = 4,
        stamina = 14,
        evade = 2.0,
        experience_points = 8,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -5,
            y_position = 22,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Tail"),
            x_position = 18,
            y_position = 45,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
-- HSP: high HP, med SP
-- ATK: med phys, zero meta
-- DEF: med phys, low meta
-- SPD: low agi, low eva
-- XPD: med XP, high drunes
--------------------------------------------------------------------------------
enemies[62] = {
    name = vt_system.Translate("Stygian Lizard"),
    stamina_icon = "data/battles/stamina_icons/enemies/stygian_lizard.png",
    battle_sprites = "data/entities/battle/enemies/stygian_lizard.png",
    sprite_width = 192,
    sprite_height = 192,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 14,
        mag_def = 4,
        stamina = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Eye"),
            x_position = 10,
            y_position = 160,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Torso"),
            x_position = -13,
            y_position = 80,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Claw"),
            x_position = -60,
            y_position = 115,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
    name = vt_system.Translate("Demonic Essence"),
    stamina_icon = "data/battles/stamina_icons/enemies/demonic_essence.png",
    battle_sprites = "data/entities/battle/enemies/demonic_essence.png",
    sprite_width = 128,
    sprite_height = 192,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 14,
        mag_def = 4,
        stamina = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Face"),
            x_position = 0,
            y_position = 166,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = -8,
            y_position = 114,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Claw"),
            x_position = -48,
            y_position = 108,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
enemies[65] = {
    name = vt_system.Translate("Red Slime"),
    stamina_icon = "data/battles/stamina_icons/enemies/red_slime.png",
    battle_sprites = "data/entities/battle/enemies/red_slime.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 14,
        mag_def = 4,
        stamina = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = -13,
            y_position = 80,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
    name = vt_system.Translate("Mushroom Zombie"),
    stamina_icon = "data/battles/stamina_icons/enemies/mushroom_zombie.png",
    battle_sprites = "data/entities/battle/enemies/mushroom_zombie.png",
    sprite_width = 64,
    sprite_height = 64,

    base_stats = {
        hit_points = 124,
        skill_points = 10,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 14,
        mag_def = 4,
        stamina = 13,
        evade = 2.0,
        experience_points = 5,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Body"),
            x_position = -13,
            y_position = 80,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
-- HSP: vhigh HP, med SP
-- ATK: high phys, zero meta
-- DEF: high phys, low meta
-- SPD: med agi, low eva
-- XPD: vhigh XP, high drunes
-- Notes: First boss in prologue module
--------------------------------------------------------------------------------
enemies[91] = {
    name = vt_system.Translate("Scorpion Goliath"),
    stamina_icon = "data/battles/stamina_icons/enemies/scorpion_goliath.png",
    battle_sprites = "data/entities/battle/enemies/scorpion_goliath.png",
    sprite_width = 512,
    sprite_height = 448,

    base_stats = {
        hit_points = 500,
        skill_points = 45,
        phys_atk = 55,
        mag_atk = 0,
        phys_def = 20,
        mag_def = 5,
        stamina = 25,
        evade = 3.0,
        experience_points = 242,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -30,
            y_position = 125,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Pincer"),
            x_position = -190,
            y_position = 120,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Leg"),
            x_position = 200,
            y_position = 160,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
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
    name = vt_system.Translate("Armored Beast"),
    stamina_icon = "data/battles/stamina_icons/enemies/armored_beast.png",
    battle_sprites = "data/entities/battle/enemies/armored_beast.png",
    sprite_width = 256,
    sprite_height = 256,

    base_stats = {
        hit_points = 122,
        skill_points = 10,
        phys_atk = 10,
        mag_atk = 0,
        phys_def = 10,
        mag_def = 4,
        stamina = 30,
        evade = 18.0,
        experience_points = 8,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Mesosoma"),
            x_position = -6,
            y_position = 23,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Pincers"),
            x_position = 7,
            y_position = 26,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Legs"),
            x_position = 7,
            y_position = 26,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[101] = {
    name = vt_system.Translate("Daemarbora"),
    stamina_icon = "data/battles/stamina_icons/enemies/daemarbora.png",
    battle_sprites = "data/entities/battle/enemies/daemarbora.png",
    sprite_width = 128,
    sprite_height = 128,

    initial_stats = {
        hit_points = 80,
        skill_points = 20,
        experience_points = 15,
        phys_atk = 12,
        mag_atk = 20,
        phys_def = 15,
        mag_def = 7,
        stamina = 8,
        evade = 1.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Trunk"),
            x_position = -6,
            y_position = 36,
            phys_def_modifier = 0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Branches"),
            x_position = 0,
            y_position = 80,
            phys_def_modifier = 0,
            mag_def_modifier = 0,
            evade_modifier = 1.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[102] = {
    name = vt_system.Translate("Aerocephal"),
    stamina_icon = "data/battles/stamina_icons/enemies/aerocephal.png",
    battle_sprites = "data/entities/battle/enemies/aerocephal.png",
    sprite_width = 192,
    sprite_height = 192,

    initial_stats = {
        hit_points = 90,
        skill_points = 10,
        experience_points = 12,
        phys_atk = 8,
        mag_atk = 0,
        phys_def = 7,
        mag_def = 4,
        stamina = 20,
        evade = 10.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Forehead"),
            x_position = -6,
            y_position = 127,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Orifice"),
            x_position = -1,
            y_position = 77,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[103] = {
    name = vt_system.Translate("Arcana Drake"),
    stamina_icon = "data/battles/stamina_icons/enemies/arcana_drake.png",
    battle_sprites = "data/entities/battle/enemies/arcana_drake.png",
    sprite_width = 192,
    sprite_height = 256,

    initial_stats = {
        hit_points = 85,
        skill_points = 10,
        experience_points = 45,
        phys_atk = 20,
        mag_atk = 0,
        phys_def = 8,
        mag_def = 5,
        stamina = 15,
        evade = 2.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -6,
            y_position = 167,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Body"),
            x_position = -1,
            y_position = 111,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Tail"),
            x_position = -74,
            y_position = 146,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[104] = {
    name = vt_system.Translate("Nagaruda"),
    stamina_icon = "data/battles/stamina_icons/enemies/nagaruda.png",
    battle_sprites = "data/entities/battle/enemies/nagaruda.png",
    sprite_width = 192,
    sprite_height = 256,

    initial_stats = {
        hit_points = 90,
        skill_points = 10,
        experience_points = 18,
        phys_atk = 10,
        mag_atk = 0,
        phys_def = 8,
        mag_def = 4,
        stamina = 12,
        evade = 2.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -26,
            y_position = 165,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Abdomen"),
            x_position = -36,
            y_position = 115,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Tail"),
            x_position = -26,
            y_position = 65,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[105] = {
    name = vt_system.Translate("Deceleon"),
    stamina_icon = "data/battles/stamina_icons/enemies/deceleon.png",
    battle_sprites = "data/entities/battle/enemies/deceleon.png",
    sprite_width = 256,
    sprite_height = 256,

    initial_stats = {
        hit_points = 100,
        skill_points = 10,
        experience_points = 18,
        phys_atk = 15,
        mag_atk = 0,
        phys_def = 20,
        mag_def = 4,
        stamina = 4,
        evade = 1.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -104,
            y_position = 226,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Chest"),
            x_position = -106,
            y_position = 190,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Arm"),
            x_position = -56,
            y_position = 155,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        },
        [4] = {
            name = vt_system.Translate("Legs"),
            x_position = -106,
            y_position = 105,
            phys_def_modifier = 0.0,
            mag_def_modifier = 0.0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}


enemies[106] = {
    name = vt_system.Translate("Aurum Drakueli"),
    stamina_icon = "data/battles/stamina_icons/enemies/aurum_drakueli.png",
    battle_sprites = "data/entities/battle/enemies/aurum_drakueli.png",
    sprite_width = 320,
    sprite_height = 256,


    initial_stats = {
        hit_points = 120,
        skill_points = 10,
        experience_points = 20,
        phys_atk = 22,
        mag_atk = 0,
        phys_def = 8,
        mag_def = 4,
        stamina = 18,
        evade = 2.0,
        drunes = 0
    },

    attack_points = {
        [1] = {
            name = vt_system.Translate("Head"),
            x_position = -4,
            y_position = 222,
            phys_def_modifier = 0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        },
        [2] = {
            name = vt_system.Translate("Chest"),
            x_position = 39,
            y_position = 155,
            phys_def_modifier = 0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        },
        [3] = {
            name = vt_system.Translate("Arm"),
            x_position = 82,
            y_position = 143,
            phys_def_modifier = 0,
            mag_def_modifier = 0,
            evade_modifier = 0.0
        }
    },

    skills = {

    },

    drop_objects = {

    }
}
