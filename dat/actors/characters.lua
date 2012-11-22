------------------------------------------------------------------------------[[
-- Filename: characters.lua
--
-- Description: This file contains the definitions of all characters.
-- When a new character is added to the party, this file
-- is accessed and the character is created using the data loaded from this file.
------------------------------------------------------------------------------]]

-- All character definitions are stored in this table
characters = {}

characters[BRONANN] = {
	name = hoa_system.Translate("Bronann"),
	portrait = "img/portraits/bronann.png",
	--full_portrait = "img/portraits/bronann_full.png",
	battle_portraits = "img/portraits/battle/bronann_damage.png",
	stamina_icon = "img/icons/actors/characters/bronann.png",
	map_sprite_name = "Bronann",

	battle_animations = {
		idle = "img/sprites/battle/characters/bronann_idle.lua",
		run = "img/sprites/battle/characters/bronann_run.lua",
		attack = "img/sprites/battle/characters/bronann_attack.lua",
		dodge = "img/sprites/battle/characters/bronann_dodge.lua",
		victory = "img/sprites/battle/characters/bronann_victory.lua",
		hurt = "img/sprites/battle/characters/bronann_hurt.lua",
		dying = "img/sprites/battle/characters/bronann_hurt.lua",
		dead = "img/sprites/battle/characters/bronann_dead.lua",
		revive = "img/sprites/battle/characters/bronann_idle.lua",
		item = "img/sprites/battle/characters/bronann_idle.lua",
		magic_prepare = "img/sprites/battle/characters/bronann_magic_prepare.lua",
		magic_cast = "img/sprites/battle/characters/bronann_magic_cast.lua",
		jump_forward = "img/sprites/battle/characters/bronann_jump_forward.lua",
		jump_backward = "img/sprites/battle/characters/bronann_jump_backward.lua"

	},

	initial_stats = {
		experience_level = 1,
		experience_points = 0,
		max_hit_points = 62,
		max_skill_points = 8,
		strength = 12,
		vigor = 4,
		fortitude = 15,
		protection = 6,
		agility = 30,
		evade = 5.0,
		weapon = 0,
		head_armor = 0,
		torso_armor = 30001,
		arm_armor = 0,
		leg_armor = 50001
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},
    
    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
            100, 112, 126, 142, 161, 183, 209, 238, 273, 315,
            363, 421, 490, 572, 670, 788, 931, 1105, 1316, 1575
        },

        hit_points = {
            5, 5, 5, 5, 13, 13, 13, 13, 21, 21,
            21, 21, 29, 29, 29, 29, 37, 37, 37, 37
        },
    
        skill_points = {
            1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 4, 4, 4, 4, 4    
        },

        strength = {
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
            3, 3, 4, 4, 4, 4, 4, 4, 5, 5
        },

        vigor = {
            1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
            2, 2, 2, 2, 3, 3, 3, 3, 3, 3
        },

        fortitude = {
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
            3, 3, 4, 4, 4, 4, 4, 4, 5, 5
        },

        protection = {
            1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
            2, 2, 2, 2, 3, 3, 3, 3, 3, 3
        },

        agility = {
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0
        }
    },

    skills = { [1] = 1, [3] = 2, [8] = 3, [12] = 20001, [15] = 4 }

} -- characters[BRONANN]

characters[KALYA] = {
	name = hoa_system.Translate("Kalya"),
	portrait = "img/portraits/kalya.png",
	full_portrait = "img/portraits/kalya_full.png",
	battle_portraits = "img/portraits/battle/kalya_damage.png",
	stamina_icon = "img/icons/actors/characters/kalya.png",
	map_sprite_name = "Kalya",

	battle_animations = {
		idle = "img/sprites/battle/characters/kalya_idle.lua",
		run = "img/sprites/battle/characters/kalya_run.lua",
		attack = "img/sprites/battle/characters/kalya_attack.lua",
		dodge = "img/sprites/battle/characters/kalya_dodge.lua",
		victory = "img/sprites/battle/characters/kalya_victory.lua",
		hurt = "img/sprites/battle/characters/kalya_hurt.lua",
		dying = "img/sprites/battle/characters/kalya_hurt.lua",
		dead = "img/sprites/battle/characters/kalya_dead.lua",
		revive = "img/sprites/battle/characters/kalya_idle.lua",
		item = "img/sprites/battle/characters/kalya_idle.lua",
		magic_prepare = "img/sprites/battle/characters/kalya_magic_prepare.lua",
		magic_cast = "img/sprites/battle/characters/kalya_magic_cast.lua"
	},

	initial_stats = {
		experience_level = 1,
		experience_points = 0,
		max_hit_points = 56,
		max_skill_points = 12,
		strength = 10,
		vigor = 4,
		fortitude = 15,
		protection = 6,
		agility = 35,
		evade = 5.0,
		weapon = 11001,
		head_armor = 20001,
		torso_armor = 30002,
		arm_armor = 0,
		leg_armor = 50001
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
            98, 110, 124, 139, 158, 179, 204, 233, 267, 307,
            354, 410, 476, 556, 651, 765, 904, 1071, 1275, 1525
        },

        hit_points = {
            5, 5, 5, 5, 5, 13, 13, 13, 13, 13,
            21, 21, 21, 21, 21, 29, 29, 29, 29, 29
        },
    
        skill_points = {
            1, 1, 1, 1, 1, 3, 3, 3, 3, 3,
            5, 5, 5, 5, 5, 7, 7, 7, 7, 7
        },

        strength = {
            2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
            3, 3, 3, 3, 3, 3, 4, 4, 4, 4
        },

        vigor = {
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
            3, 3, 4, 4, 4, 4, 4, 4, 5, 5
        },

        fortitude = {
            2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
            3, 3, 3, 3, 3, 3, 4, 4, 4, 4
        },

        protection = {
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
            3, 3, 4, 4, 4, 4, 4, 4, 5, 5
        },

        agility = {
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.45, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.45, 0.0, 0.0, 0.0
        }
    },

	skills = { [1] = 5, [4] = 10001, [12] = 20001 }
} -- characters[KALYA]

characters[SYLVE] = {
	name = hoa_system.Translate("Sylve"),
	portrait = "img/portraits/sylve.png",
	full_portrait = "img/portraits/sylve_full.png",
	battle_portraits = "img/portraits/battle/sylve_damage.png",
	stamina_icon = "img/icons/actors/characters/sylve.png",
	map_sprite_name = "Sylve",

	battle_animations = {
		idle = "img/sprites/battle/characters/sylve_idle.lua",
		run = "img/sprites/battle/characters/sylve_run.lua",
		attack = "img/sprites/battle/characters/sylve_run.lua",
		dodge = "img/sprites/battle/characters/sylve_idle.lua",
		victory = "img/sprites/battle/characters/sylve_idle.lua",
		dying = "img/sprites/battle/characters/sylve_dead.lua",
		dead = "img/sprites/battle/characters/sylve_dead.lua",
		revive = "img/sprites/battle/characters/sylve_idle.lua",
		item = "img/sprites/battle/characters/sylve_idle.lua",
		magic_prepare = "img/sprites/battle/characters/sylve_idle.lua",
		magic_cast = "img/sprites/battle/characters/sylve_idle.lua"
	},

	initial_stats = {
		experience_level = 1,
		experience_points = 0,
		max_hit_points = 62,
		max_skill_points = 8,
		strength = 12,
		vigor = 4,
		fortitude = 15,
		protection = 6,
		agility = 30,
		evade = 5.0,
		weapon = 10001,
		head_armor = 20011,
		torso_armor = 30011,
		arm_armor = 40001,
		leg_armor = 50001
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
            99, 111, 125, 141, 160, 181, 206, 236, 270, 311,
            359, 415, 483, 564, 660, 777, 917, 1088, 1296, 1550
        },

        hit_points = {
            5, 5, 5, 5, 5, 5, 14, 14, 14, 14,
            14, 14, 23, 23, 23, 23, 23, 23, 32, 32
        },
    
        skill_points = {
            1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 4, 4, 4, 4, 4
        },

        strength = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        vigor = {
            1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3
        },

        fortitude = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        protection = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        agility = {
            1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
            1, 0, 1, 1, 1, 0, 1, 1, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.6, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.0, 0.0
        }
    },

	skills = { [1] = 6 }
} -- characters[SYLVE]

characters[THANIS] = {
	name = hoa_system.Translate("Thanis"),
	portrait = "img/portraits/thanis.png",
	full_portrait = "img/portraits/thanis_full.png",
	battle_portraits = "img/portraits/battle/thanis_damage.png",
	stamina_icon = "img/icons/actors/characters/thanis.png",
	map_sprite_name = "Thanis",

	battle_animations = {
		idle = "img/sprites/battle/characters/thanis_idle.lua",
		run = "img/sprites/battle/characters/thanis_run.lua",
		run_left = "img/sprites/battle/characters/thanis_run_left.lua",
		attack = "img/sprites/battle/characters/thanis_attack.lua",
		dodge = "img/sprites/battle/characters/thanis_idle.lua",
		victory = "img/sprites/battle/characters/thanis_victory.lua",
		dying = "img/sprites/battle/characters/thanis_dead.lua",
		dead = "img/sprites/battle/characters/thanis_dead.lua",
		revive = "img/sprites/battle/characters/thanis_idle.lua",
		item = "img/sprites/battle/characters/thanis_idle.lua",
		magic_prepare = "img/sprites/battle/characters/thanis_idle.lua",
		magic_cast = "img/sprites/battle/characters/thanis_idle.lua"
	},

	initial_stats = {
		experience_level = 18,
		experience_points = 6728,
		max_hit_points = 237,
		max_skill_points = 25,
		strength = 45,
		vigor = 21,
		fortitude = 47,
		protection = 23,
		agility = 46,
		evade = 7.0,
		weapon = 10003,
		head_armor = 20013,
		torso_armor = 30013,
		arm_armor = 40013,
		leg_armor = 50012
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
            101, 113, 128, 144, 163, 185, 211, 241, 277, 319,
            368, 426, 496, 579, 679, 800, 945, 1122, 1337, 1600
        },

        hit_points = {
            5, 5, 5, 5, 14, 14, 14, 14, 23, 23,
            23, 23, 32, 32, 32, 32, 41, 41, 41, 41
        },
    
        skill_points = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        strength = {
            2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 5, 5, 5, 5, 5
        },

        vigor = {
            1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
            2, 2, 2, 2, 3, 3, 3, 3, 3, 3
        },

        fortitude = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        protection = {
            1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3
        },

        agility = {
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.35, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.35, 0.0, 0.0, 0.0
        }
    },

	skills = { [1] = 1, [3] = 2, [4] = 10001, [8] = 3, [12] = 20001, [15] = 4, [17] = 7 }
} -- characters[THANIS]

--[[

characters[MARK] = {
	name = hoa_system.Translate("Mark"),

	battle_animations = {
		idle = "img/sprites/battle/characters/soldier_idle.lua",
		run = "img/sprites/battle/characters/soldier_run.lua",
		attack = "img/sprites/battle/characters/soldier_attack.lua",
		dodge = "img/sprites/battle/characters/soldier_idle.lua",
		victory = "img/sprites/battle/characters/soldier_victory.lua",
		dying = "img/sprites/battle/characters/soldier_dead.lua",
		dead = "img/sprites/battle/characters/soldier_dead.lua",
		revive = "img/sprites/battle/characters/soldier_idle.lua",
		item = "img/sprites/battle/characters/soldier_idle.lua",
		magic_prepare = "img/sprites/battle/characters/soldier_idle.lua",
		magic_cast = "img/sprites/battle/characters/soldier_idle.lua"
	},

	initial_stats = {
		experience_level = 7,
		experience_points = 1128,
		max_hit_points = 98,
		max_skill_points = 13,
		strength = 21,
		vigor = 8,
		fortitude = 24,
		protection = 10,
		agility = 35,
		evade = 5.2,
		weapon = 10002,
		head_armor = 20002,
		torso_armor = 30002,
		arm_armor = 40002,
		leg_armor = 50001
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

	skills = { [1] = 1, [3] = 2, [4] = 10001 }
} -- characters[MARK]



characters[DESTER] = {
	name = hoa_system.Translate("Dester"),

	battle_animations = {
		idle = "img/sprites/battle/characters/soldier_idle.lua",
		run = "img/sprites/battle/characters/soldier_run.lua",
		attack = "img/sprites/battle/characters/soldier_attack.lua",
		dodge = "img/sprites/battle/characters/soldier_idle.lua",
		victory = "img/sprites/battle/characters/soldier_victory.lua",
		dying = "img/sprites/battle/characters/soldier_dead.lua",
		dead = "img/sprites/battle/characters/soldier_dead.lua",
		revive = "img/sprites/battle/characters/soldier_idle.lua",
		item = "img/sprites/battle/characters/soldier_idle.lua",
		magic_prepare = "img/sprites/battle/characters/soldier_idle.lua",
		magic_cast = "img/sprites/battle/characters/soldier_idle.lua"
	},

	initial_stats = {
		experience_level = 9,
		experience_points = 1845,
		max_hit_points = 120,
		max_skill_points = 15,
		strength = 25,
		vigor = 12,
		fortitude = 26,
		protection = 14,
		agility = 38,
		evade = 5.3,
		weapon = 10002,
		head_armor = 20002,
		torso_armor = 30002,
		arm_armor = 40002,
		leg_armor = 50001
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

	skills = { [1] = 1, [3] = 2, [4] = 10001, [8] = 3 }
} -- characters[DESTER]



characters[LUKAR] = {
	name = hoa_system.Translate("Lukar"),

	battle_animations = {
		idle = "img/sprites/battle/characters/knight_idle.lua",
		run = "img/sprites/battle/characters/knight_run.lua",
		attack = "img/sprites/battle/characters/knight_attack.lua",
		dodge = "img/sprites/battle/characters/knight_idle.lua",
		victory = "img/sprites/battle/characters/knight_victory.lua",
		dying = "img/sprites/battle/characters/knight_dead.lua",
		dead = "img/sprites/battle/characters/knight_dead.lua",
		revive = "img/sprites/battle/characters/knight_idle.lua",
		item = "img/sprites/battle/characters/knight_idle.lua",
		magic_prepare = "img/sprites/battle/characters/knight_idle.lua",
		magic_cast = "img/sprites/battle/characters/knight_idle.lua"
	},

	initial_stats = {
		experience_level = 18,
		experience_points = 6728,
		max_hit_points = 237,
		max_skill_points = 25,
		strength = 45,
		vigor = 21,
		fortitude = 47,
		protection = 23,
		agility = 46,
		evade = 7.0,
		weapon = 10003,
		head_armor = 20003,
		torso_armor = 30003,
		arm_armor = 40003,
		leg_armor = 50002
	},

	attack_points = {
		[hoa_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
			name = hoa_system.Translate("Head"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = -0.20,
			protection_modifier = 0.25,
			evade_modifier = 0.50,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_VIGOR_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
			name = hoa_system.Translate("Torso"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.40,
			protection_modifier = 0.10,
			evade_modifier = -0.20
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
			name = hoa_system.Translate("Arms"),
			x_position = 31,
			y_position = 54,
			fortitude_modifier = 0.10,
			protection_modifier = 0.00,
			evade_modifier = 0.10,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_STRENGTH_LOWER] = 10.0 }
		},
		[hoa_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
			name = hoa_system.Translate("Legs"),
			x_position = 37,
			y_position = 34,
			fortitude_modifier = 0.20,
			protection_modifier = 0.20,
			evade_modifier = 0.05,
			status_effects = { [hoa_global.GameGlobal.GLOBAL_STATUS_AGILITY_LOWER] = 10.0 }
		}
	},

	skills = { [1] = 1, [3] = 2, [4] = 10001, [8] = 3, [12] = 20001, [15] = 4, [17] = 20002 }
} -- characters[LUKAR]

]]--

------------------------------------------------------------------------------[[
-- \brief Called everytime a character reaches a new experience level.
-- \param character A pointer to the GlobalCharacter who has gained the new level
--
-- Before this function is called, the character should already have their
-- _experience_level member incremented. What this function does is determine
-- the amount that each stat will grow by on the next level and if any new
-- skills are learned on this level.
------------------------------------------------------------------------------]]
function DetermineGrowth(character)
	local new_level = character:GetExperienceLevel();        -- The value of the character's new XP level
	local character_table = characters[character:GetID()];   -- Reference to the character's definition table
	local growth_table = nil;                                -- Reference to the table containing the character's growth stats
	local new_skills = nil;                                  -- Reference to the number or table of the new skills learned
	local character_growth = character:GetGrowth();          -- Reference to the growth object contained within the character

	if (character_table == nil) then
		print("LUA ERROR: characters.lua::DetermineGrowth() failed because the character's ID was invalid");
		return;
	end

    growth_table = character_table["growth"];
	if (growth_table == nil) then
		print("LUA ERROR: characters.lua::DetermineGrowth() failed because no growth table for the character was found");
		return;
	end

    character:SetExperienceForNextLevel(growth_table["experience_for_next_level"][new_level]);

	-- Calculate how much the character's stats are going to grow by the next level
	character_growth._hit_points_growth = growth_table["hit_points"][new_level];
	character_growth._skill_points_growth = growth_table["skill_points"][new_level];
	character_growth._strength_growth = growth_table["strength"][new_level];
	character_growth._vigor_growth = growth_table["vigor"][new_level];
	character_growth._fortitude_growth = growth_table["fortitude"][new_level];
	character_growth._protection_growth = growth_table["protection"][new_level];
	character_growth._agility_growth = growth_table["agility"][new_level];
	character_growth._evade_growth = growth_table["evade"][new_level];

	-- Add any skills that the character gains at this level
    new_skills = character_table["skills"][new_level]

    -- Case 1: no new skills are learned
    if (type(new_skills) == nil) then
        return;
    -- Case 2: one new skill is learned
	elseif (type(new_skills) == "number") then
		character_growth:_AddSkill(new_skills);
    -- Case 3: multiple new skills are learned
	elseif (type(new_skills) == "table") then
		for i, skill in ipairs(new_skills) do
			character_growth:_AddSkill(skill);
		end
	else
		print("LUA ERROR: characters.lua::DetermineGrowth() failed because of an unexpected skill table key type");
	end
end -- function DetermineGrowth(character)
