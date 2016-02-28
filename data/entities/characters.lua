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
    name = vt_system.Translate("Bronann"),
    portrait = "data/entities/portraits/bronann.png",
    full_portrait = "data/entities/portraits/bronann_full.png",
    battle_portraits = "data/entities/portraits/battle/bronann_damage.png",
    stamina_icon = "data/battles/stamina_icons/characters/bronann.png",
    map_sprite_name = "Bronann",
    special_skill_category_name = vt_system.Translate("Holy"),
    special_skill_category_icon = "data/gui/battle/holy.png",

    battle_animations = {
        idle = "data/entities/battle/characters/bronann/bronann_idle.lua",
        run = "data/entities/battle/characters/bronann/bronann_run.lua",
        run_after_victory = "data/entities/battle/characters/bronann/bronann_run_after_victory.lua",
        attack = "data/entities/battle/characters/bronann/bronann_attack.lua",
        attack_forward_thrust = "data/entities/battle/characters/bronann/bronann_attack_forward_thrust.lua",
        dodge = "data/entities/battle/characters/bronann/bronann_dodge.lua",
        victory = "data/entities/battle/characters/bronann/bronann_victory.lua",
        hurt = "data/entities/battle/characters/bronann/bronann_hurt.lua",
        poor = "data/entities/battle/characters/bronann/bronann_kneeling.lua",
        dying = "data/entities/battle/characters/bronann/bronann_kneeling.lua",
        dead = "data/entities/battle/characters/bronann/bronann_dead.lua",
        revive = "data/entities/battle/characters/bronann/bronann_kneeling.lua",
        item = "data/entities/battle/characters/bronann/bronann_idle.lua",
        magic_prepare = "data/entities/battle/characters/bronann/bronann_magic_prepare.lua",
        magic_cast = "data/entities/battle/characters/bronann/bronann_magic_cast.lua",
        jump_forward = "data/entities/battle/characters/bronann/bronann_jump_forward.lua",
        jump_backward = "data/entities/battle/characters/bronann/bronann_jump_backward.lua"
    },

    initial_stats = {
        experience_level = 1,
        experience_points = 0,
        max_hit_points = 62,
        max_skill_points = 8,
        phys_atk = 12,
        mag_atk = 4,
        phys_def = 15,
        mag_def = 6,
        stamina = 30,
        evade = 5.0,
        weapon = 0,
        head_armor = 0,
        torso_armor = 30001,
        arm_armor = 0,
        leg_armor = 50001
    },

    attack_points = {
        [vt_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
            name = vt_system.Translate("Head"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = -0.20,
            mag_def_modifier = 0.25,
            evade_modifier = 0.50,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
            name = vt_system.Translate("Torso"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.40,
            mag_def_modifier = 0.10,
            evade_modifier = -0.20
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
            name = vt_system.Translate("Arms"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = 0.10,
            mag_def_modifier = 0.00,
            evade_modifier = 0.10,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
            name = vt_system.Translate("Legs"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.20,
            mag_def_modifier = 0.20,
            evade_modifier = 0.05,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
        }
    },

    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
                100,     112,     126,     142,     161,     183,     209,     238,     273,     315, --  1 - 10
                363,     421,     490,     572,     670,     788,     931,    1105,    1316,    1575, -- 11 - 20
               1894,    2287,    2775,    3383,    3980,    4697,    5563,    6610,    7881,    9430, -- 21 - 30
              11324,   13648,   16510,   20050,   24443,   29917,   36765,   45367,   56217,   69963, -- 31 - 40
              87451,  109799,  138488,  175484,  223420,  285828,  367474,  474823,  616683,  659193, -- 41 - 50
             704867,  753959,  806742,  863512,  924591,  990329, 1061105, 1137331, 1219457, 1307968, -- 51 - 60
            1403396, 1506316, 1617356, 1737200, 1866590, 2006338, 2157328, 2320522, 2496972, 2687823, -- 61 - 70
            2894328, 3117852, 3359888, 3622066, 3761437, 3906539, 4057622, 4214949, 4378792, 4549437, -- 71 - 80
            4727184, 4912347, 5105253, 5306245, 5515683, 5733942, 5961416, 6198517, 6445676, 6703346, -- 81 - 90
            6972000, 7252133, 7544265, 7848940, 8166727, 8498223, 8844055, 9204879, 9581381, 9974283, -- 91 - 100
            10384340, -- 101
        },

        hit_points = {
              5,   5,   5,   5,  13,  13,  13,  13,  21,  21,     --  2 - 11
             21,  21,  29,  29,  29,  29,  37,  37,  37,  37,     -- 12 - 21
             45,  45,  45,  45,  53,  53,  53,  53,  61,  61,     -- 22 - 31
             61,  61,  69,  69,  69,  69,  77,  77,  77,  77,     -- 32 - 41
             85,  85,  85,  85,  93,  93,  93,  93, 101, 101,     -- 42 - 51
            101, 101, 109, 109, 109, 109, 117, 117, 117, 117,     -- 52 - 61
            125, 125, 125, 125, 133, 133, 133, 133, 141, 141,     -- 62 - 71
            141, 141, 149, 149, 149, 149, 157, 157, 157, 157,     -- 72 - 81
            165, 165, 165, 165, 173, 173, 173, 173, 181, 181,     -- 82 - 91
            181, 181, 189, 189, 189, 189, 197, 197, 197, 197,     -- 92 - 101
        },

        skill_points = {
             1,  1,  1,  1,  1,  2,  2,  2,  2,  2,               --  2 - 11
             3,  3,  3,  3,  3,  4,  4,  4,  4,  4,               -- 12 - 21
             5,  5,  5,  5,  5,  6,  6,  6,  6,  6,               -- 22 - 31
             7,  7,  7,  7,  7,  8,  8,  8,  8,  8,               -- 32 - 41
             9,  9,  9,  9,  9, 10, 10, 10, 10, 10,               -- 42 - 51
            11, 11, 11, 11, 11, 12, 12, 12, 12, 12,               -- 52 - 61
            13, 13, 13, 13, 13, 14, 14, 14, 14, 14,               -- 62 - 71
            15, 15, 15, 15, 15, 16, 16, 16, 16, 16,               -- 72 - 81
            17, 17, 17, 17, 17, 18, 18, 18, 18, 18,               -- 82 - 91
            19, 19, 19, 19, 19, 20, 20, 20, 20, 20,               -- 92 - 101

        },

        phys_atk = {
             2,  2,  2,  2,  2,  2,  3,  3,  3,  3,               --  2 - 11
             3,  3,  4,  4,  4,  4,  4,  4,  5,  5,               -- 12 - 21
             5,  5,  5,  5,  6,  6,  6,  6,  6,  6,               -- 22 - 31
             7,  7,  7,  7,  7,  7,  8,  8,  8,  8,               -- 32 - 41
             8,  8,  9,  9,  9,  9,  9,  9, 10, 10,               -- 42 - 51
            10, 10, 10, 10, 11, 11, 11, 11, 11, 11,               -- 52 - 61
            12, 12, 12, 12, 12, 12, 13, 13, 13, 13,               -- 62 - 71
            13, 13, 14, 14, 14, 14, 14, 14, 15, 15,               -- 72 - 81
            15, 15, 15, 15, 16, 16, 16, 16, 16, 16,               -- 82 - 91
            17, 17, 17, 17, 17, 17, 18, 18, 18, 18,               -- 92 - 101
        },

        mag_atk = {
             1,  1,  1,  1,  1,  1,  1,  2,  2,  2,               --  2 - 11
             2,  2,  2,  2,  3,  3,  3,  3,  3,  3,               -- 12 - 21
             3,  4,  4,  4,  4,  4,  4,  4,  5,  5,               -- 22 - 31
             5,  5,  5,  5,  5,  6,  6,  6,  6,  6,               -- 32 - 41
             6,  6,  7,  7,  7,  7,  7,  7,  7,  8,               -- 42 - 51
             8,  8,  8,  8,  8,  8,  9,  9,  9,  9,               -- 52 - 61
             9,  9,  9, 10, 10, 10, 10, 10, 10, 10,               -- 62 - 71
            11, 11, 11, 11, 11, 11, 11, 12, 12, 12,               -- 72 - 81
            12, 12, 12, 12, 13, 13, 13, 13, 13, 13,               -- 82 - 91
            13, 14, 14, 14, 14, 14, 14, 14, 15, 15,               -- 92 - 101
        },

        phys_def = {
             2,  2,  2,  2,  2,  2,  3,  3,  3,  3,               --  2 - 11
             3,  3,  4,  4,  4,  4,  4,  4,  5,  5,               -- 12 - 21
             5,  5,  5,  5,  6,  6,  6,  6,  6,  6,               -- 22 - 31
             7,  7,  7,  7,  7,  7,  8,  8,  8,  8,               -- 32 - 41
             8,  8,  9,  9,  9,  9,  9,  9, 10, 10,               -- 42 - 51
            10, 10, 10, 10, 11, 11, 11, 11, 11, 11,               -- 52 - 61
            12, 12, 12, 12, 12, 12, 13, 13, 13, 13,               -- 62 - 71
            13, 13, 14, 14, 14, 14, 14, 14, 15, 15,               -- 72 - 81
            15, 15, 15, 15, 16, 16, 16, 16, 16, 16,               -- 82 - 91
            17, 17, 17, 17, 17, 17, 18, 18, 18, 18,               -- 92 - 101
        },

        mag_def = {
             1,  1,  1,  1,  1,  1,  1,  2,  2,  2,               --  2 - 11
             2,  2,  2,  2,  3,  3,  3,  3,  3,  3,               -- 12 - 21
             3,  4,  4,  4,  4,  4,  4,  4,  5,  5,               -- 22 - 31
             5,  5,  5,  5,  5,  6,  6,  6,  6,  6,               -- 32 - 41
             6,  6,  7,  7,  7,  7,  7,  7,  7,  8,               -- 42 - 51
             8,  8,  8,  8,  8,  8,  9,  9,  9,  9,               -- 52 - 61
             9,  9,  9, 10, 10, 10, 10, 10, 10, 10,               -- 62 - 71
            11, 11, 11, 11, 11, 11, 11, 12, 12, 12,               -- 72 - 81
            12, 12, 12, 12, 13, 13, 13, 13, 13, 13,               -- 82 - 91
            13, 14, 14, 14, 14, 14, 14, 14, 15, 15,               -- 92 - 101
        },

        stamina = {
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               --  2 - 11
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 12 - 21
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 22 - 31
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 32 - 41
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 42 - 51
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 52 - 61
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 62 - 71
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 72 - 81
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 82 - 91
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,               -- 92 - 101
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0,   --  2 - 11
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,   -- 12 - 21
            0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0,   -- 22 - 31
            0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,   -- 32 - 41
            0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0,   -- 42 - 51
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,   -- 52 - 61
            0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0,   -- 62 - 71
            0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,   -- 72 - 81
            0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0,   -- 82 - 91
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,   -- 92 - 101
        }
    },

    -- The default skill available when no weapon.
    bare_hands_skills = { 30001 },

    -- [character level] = skill_id learned.
    skills = { [1] = 1, [2] = 2, [8] = 3, [16] = 10003, [32] = 4, [64] = 10004, [100] = 6 }
} -- characters[BRONANN]

characters[KALYA] = {
    name = vt_system.Translate("Kalya"),
    portrait = "data/entities/portraits/kalya.png",
    full_portrait = "data/entities/portraits/kalya_full.png",
    battle_portraits = "data/entities/portraits/battle/kalya_damage.png",
    stamina_icon = "data/battles/stamina_icons/characters/kalya.png",
    map_sprite_name = "Kalya",
    special_skill_category_name = vt_system.Translate("Invocation"),
    special_skill_category_icon = "data/gui/battle/invocation.png",

    battle_animations = {
        idle = "data/entities/battle/characters/kalya/kalya_idle.lua",
        run = "data/entities/battle/characters/kalya/kalya_run.lua",
        run_after_victory = "data/entities/battle/characters/kalya/kalya_run.lua",
        attack = "data/entities/battle/characters/kalya/kalya_attack.lua",
        dodge = "data/entities/battle/characters/kalya/kalya_dodge.lua",
        victory = "data/entities/battle/characters/kalya/kalya_victory.lua",
        hurt = "data/entities/battle/characters/kalya/kalya_hurt.lua",
        poor = "data/entities/battle/characters/kalya/kalya_kneeling.lua",
        dying = "data/entities/battle/characters/kalya/kalya_kneeling.lua",
        dead = "data/entities/battle/characters/kalya/kalya_dead.lua",
        revive = "data/entities/battle/characters/kalya/kalya_kneeling.lua",
        item = "data/entities/battle/characters/kalya/kalya_idle.lua",
        magic_prepare = "data/entities/battle/characters/kalya/kalya_magic_prepare.lua",
        magic_cast = "data/entities/battle/characters/kalya/kalya_magic_cast.lua",
        throw_stone = "data/entities/battle/characters/kalya/kalya_throw_stone.lua" -- bare hand attack
    },

    initial_stats = {
        experience_level = 1,
        experience_points = 0,
        max_hit_points = 56,
        max_skill_points = 12,
        phys_atk = 10,
        mag_atk = 4,
        phys_def = 15,
        mag_def = 6,
        stamina = 35,
        evade = 5.0,
        weapon = 11001,
        head_armor = 20001,
        torso_armor = 30002,
        arm_armor = 0,
        leg_armor = 50001
    },

    attack_points = {
        [vt_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
            name = vt_system.Translate("Head"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = -0.20,
            mag_def_modifier = 0.25,
            evade_modifier = 0.50,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
            name = vt_system.Translate("Torso"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.40,
            mag_def_modifier = 0.10,
            evade_modifier = -0.20
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
            name = vt_system.Translate("Arms"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = 0.10,
            mag_def_modifier = 0.00,
            evade_modifier = 0.10,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
            name = vt_system.Translate("Legs"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.20,
            mag_def_modifier = 0.20,
            evade_modifier = 0.05,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
        }
    },

    -- Begin character growth tables. Every line within these tables contains 10 elements to represent the stat growth for every 10 levels
    growth = {
        experience_for_next_level = {
                 98,     110,     124,     139,     158,     179,     204,     233,     267,     307,  --  1 - 10
                354,     410,     476,     556,     651,     765,     904,    1071,    1275,    1525, --  11 - 20
               1832,    2211,    2680,    3264,    3838,    4526,    5356,    6359,    7577,    9059, --  21 - 30
              10869,   13089,   15822,   19197,   23383,   28594,   35108,   43282,   53584,   66621, --  31 - 40
              83193,  104349,  131479,  166430,  211669,  270501,  347386,  448362,  581649,  621562, --  41 - 50
             664434,  710500,  760015,  813254,  870517,  932129,  998443, 1069843, 1146744, 1229600, --  51 - 60
            1318903, 1415187, 1519035, 1631082, 1752016, 1882590, 2023623, 2176006, 2340715, 2518809, --  61 - 70
            2711447, 2919895, 3145531, 3389866, 3519718, 3654889, 3795608, 3942118, 4094670, 4253529, --  71 - 80
            4418973, 4591289, 4770781, 4957765, 5152573, 5355551, 5567064, 5787490, 6017228, 6256695, --  81 - 90
            6506326, 6766579, 7037933, 7320887, 7615968, 7923726, 8244736, 8579603, 8928960, 9293469, --  91 - 100
            9673827, -- 101
        },

        hit_points = {
              5,   5,   5,   5,   5,  13,  13,  13,  13,  13,     --  2 - 11
             21,  21,  21,  21,  21,  29,  29,  29,  29,  29,     --  12 - 21
             37,  37,  37,  37,  37,  45,  45,  45,  45,  45,     --  22 - 31
             53,  53,  53,  53,  53,  61,  61,  61,  61,  61,     --  32 - 41
             69,  69,  69,  69,  69,  77,  77,  77,  77,  77,     --  42 - 51
             85,  85,  85,  85,  85,  93,  93,  93,  93,  93,     --  52 - 61
            101, 101, 101, 101, 101, 109, 109, 109, 109, 109,     --  62 - 71
            117, 117, 117, 117, 117, 125, 125, 125, 125, 125,     --  72 - 81
            133, 133, 133, 133, 133, 141, 141, 141, 141, 141,     --  82 - 91
            149, 149, 149, 149, 149, 157, 157, 157, 157, 157,     --  92 - 101
        },

        skill_points = {
             1,  1,  1,  1,  1,  3,  3,  3,  3,  3,               --   2 - 11
             5,  5,  5,  5,  5,  7,  7,  7,  7,  7,               --   12 - 21
             9,  9,  9,  9,  9, 11, 11, 11, 11, 11,               --   22 - 31
            13, 13, 13, 13, 13, 15, 15, 15, 15, 15,               --   32 - 41
            17, 17, 17, 17, 17, 19, 19, 19, 19, 19,               --   42 - 51
            21, 21, 21, 21, 21, 23, 23, 23, 23, 23,               --   52 - 61
            25, 25, 25, 25, 25, 27, 27, 27, 27, 27,               --   62 - 71
            29, 29, 29, 29, 29, 31, 31, 31, 31, 31,               --   72 - 81
            33, 33, 33, 33, 33, 35, 35, 35, 35, 35,               --   82 - 91
            37, 37, 37, 37, 37, 39, 39, 39, 39, 39,               --   92 - 101
        },

        phys_atk = {
             2,  2,  2,  2,  2,  2,  2,  2,  3,  3,               --   2 - 11
             3,  3,  3,  3,  3,  3,  4,  4,  4,  4,               --  12 - 21
             4,  4,  4,  4,  5,  5,  5,  5,  5,  5,               --  22 - 31
             5,  5,  6,  6,  6,  6,  6,  6,  6,  6,               --  32 - 41
             7,  7,  7,  7,  7,  7,  7,  7,  8,  8,               --  42 - 51
             8,  8,  8,  8,  8,  8,  9,  9,  9,  9,               --  52 - 61
             9,  9,  9,  9, 10, 10, 10, 10, 10, 10,               --  62 - 71
            10, 10, 11, 11, 11, 11, 11, 11, 11, 11,               --  72 - 81
            12, 12, 12, 12, 12, 12, 12, 12, 13, 13,               --  82 - 91
            13, 13, 13, 13, 13, 13, 14, 14, 14, 14,               --  92 - 101
        },

        mag_atk = {
             2,  2,  2,  2,  2,  2,  3,  3,  3,  3,               --  2 - 11
             3,  3,  4,  4,  4,  4,  4,  4,  5,  5,               -- 12 - 21
             5,  5,  5,  5,  6,  6,  6,  6,  6,  6,               -- 22 - 31
             7,  7,  7,  7,  7,  7,  8,  8,  8,  8,               -- 32 - 41
             8,  8,  9,  9,  9,  9,  9,  9, 10, 10,               -- 42 - 51
            10, 10, 10, 10, 11, 11, 11, 11, 11, 11,               -- 52 - 61
            12, 12, 12, 12, 12, 12, 13, 13, 13, 13,               -- 62 - 71
            13, 13, 14, 14, 14, 14, 14, 14, 15, 15,               -- 72 - 81
            15, 15, 15, 15, 16, 16, 16, 16, 16, 16,               -- 82 - 91
            17, 17, 17, 17, 17, 17, 18, 18, 18, 18,               -- 92 - 101
        },

        phys_def = {
             2,  2,  2,  2,  2,  2,  2,  2,  3,  3,               --   2 - 11
             3,  3,  3,  3,  3,  3,  4,  4,  4,  4,               --  12 - 21
             4,  4,  4,  4,  5,  5,  5,  5,  5,  5,               --  22 - 31
             5,  5,  6,  6,  6,  6,  6,  6,  6,  6,               --  32 - 41
             7,  7,  7,  7,  7,  7,  7,  7,  8,  8,               --  42 - 51
             8,  8,  8,  8,  8,  8,  9,  9,  9,  9,               --  52 - 61
             9,  9,  9,  9, 10, 10, 10, 10, 10, 10,               --  62 - 71
            10, 10, 11, 11, 11, 11, 11, 11, 11, 11,               --  72 - 81
            12, 12, 12, 12, 12, 12, 12, 12, 13, 13,               --  82 - 91
            13, 13, 13, 13, 13, 13, 14, 14, 14, 14,               --  92 - 101
        },

        mag_def = {
             2,  2,  2,  2,  2,  2,  3,  3,  3,  3,               --  2 - 11
             3,  3,  4,  4,  4,  4,  4,  4,  5,  5,               -- 12 - 21
             5,  5,  5,  5,  6,  6,  6,  6,  6,  6,               -- 22 - 31
             7,  7,  7,  7,  7,  7,  8,  8,  8,  8,               -- 32 - 41
             8,  8,  9,  9,  9,  9,  9,  9, 10, 10,               -- 42 - 51
            10, 10, 10, 10, 11, 11, 11, 11, 11, 11,               -- 52 - 61
            12, 12, 12, 12, 12, 12, 13, 13, 13, 13,               -- 62 - 71
            13, 13, 14, 14, 14, 14, 14, 14, 15, 15,               -- 72 - 81
            15, 15, 15, 15, 16, 16, 16, 16, 16, 16,               -- 82 - 91
            17, 17, 17, 17, 17, 17, 18, 18, 18, 18,               -- 92 - 101
        },

        stamina = {
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  2 - 11
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  12 - 21
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  22 - 31
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  32 - 41
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  42 - 51
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  52 - 61
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  62 - 71
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  72 - 81
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  82 - 91
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0,               --  92 - 101
        },

        evade = {
             0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.45, 0.0, --  2 - 11
             0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0.45,  0.0,  0.0,  0.0, --  12 - 21
             0.0,  0.0,  0.0,  0.0,  0.45, 0.0,  0.0,  0.0,  0.0,  0.0, --  22 - 31
             0.0,  0.0, 0.45,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, --  32 - 41
            0.45,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0.45,  0.0, --  42 - 51
             0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0.45,  0.0,  0.0,  0.0, --  52 - 61
             0.0,  0.0,  0.0,  0.0,  0.45, 0.0,  0.0,  0.0,  0.0,  0.0, --  62 - 71
             0.0,  0.0, 0.45,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, --  72 - 81
            0.45,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0.45,  0.0, --  82 - 91
             0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0.45,  0.0,  0.0,  0.0, --  92 - 101
        }
    },

    -- The default skill available when no weapon.
    bare_hands_skills = { 30002 },

    skills = { [1] = 5, [3] = 10001, [7] = 10002, [14] = 101, [28] = 102, [56] = 10005, [100] = 10006 }
} -- characters[KALYA]

characters[SYLVE] = {
    name = vt_system.Translate("Sylve"),
    portrait = "data/entities/portraits/sylve.png",
    full_portrait = "data/entities/portraits/sylve_full.png",
    battle_portraits = "data/entities/portraits/battle/sylve_damage.png",
    stamina_icon = "data/battles/stamina_icons/characters/sylve.png",
    map_sprite_name = "Sylve",
    special_skill_category_name = vt_system.Translate("Mist"),
    special_skill_category_icon = "data/gui/battle/mist.png",

    battle_animations = {
        idle = "data/entities/battle/characters/sylve_idle.lua",
        run = "data/entities/battle/characters/sylve_run.lua",
        run_after_victory = "data/entities/battle/characters/sylve_run.lua",
        attack = "data/entities/battle/characters/sylve_run.lua",
        dodge = "data/entities/battle/characters/sylve_idle.lua",
        victory = "data/entities/battle/characters/sylve_idle.lua",
        dying = "data/entities/battle/characters/sylve_dead.lua",
        dead = "data/entities/battle/characters/sylve_dead.lua",
        revive = "data/entities/battle/characters/sylve_idle.lua",
        item = "data/entities/battle/characters/sylve_idle.lua",
        magic_prepare = "data/entities/battle/characters/sylve_idle.lua",
        magic_cast = "data/entities/battle/characters/sylve_idle.lua"
    },

    initial_stats = {
        experience_level = 1,
        experience_points = 0,
        max_hit_points = 62,
        max_skill_points = 8,
        phys_atk = 12,
        mag_atk = 4,
        phys_def = 15,
        mag_def = 6,
        stamina = 30,
        evade = 5.0,
        weapon = 0,
        head_armor = 20011,
        torso_armor = 30011,
        arm_armor = 40001,
        leg_armor = 50001
    },

    attack_points = {
        [vt_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
            name = vt_system.Translate("Head"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = -0.20,
            mag_def_modifier = 0.25,
            evade_modifier = 0.50,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
            name = vt_system.Translate("Torso"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.40,
            mag_def_modifier = 0.10,
            evade_modifier = -0.20
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
            name = vt_system.Translate("Arms"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = 0.10,
            mag_def_modifier = 0.00,
            evade_modifier = 0.10,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
            name = vt_system.Translate("Legs"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.20,
            mag_def_modifier = 0.20,
            evade_modifier = 0.05,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
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

        phys_atk = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        mag_atk = {
            1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3
        },

        phys_def = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        mag_def = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        stamina = {
            1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
            1, 0, 1, 1, 1, 0, 1, 1, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.6, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.0, 0.0
        }
    },

    -- The default skill available when no weapon.
    bare_hands_skills = { 30001 },

    skills = { [1] = 200, [2] = 201, [10] = 202, [20] = 30003, [40] = 10007, [80] = 10008, [100] = 203 }
} -- characters[SYLVE]

characters[THANIS] = {
    name = vt_system.Translate("Thanis"),
    portrait = "data/entities/portraits/thanis.png",
    full_portrait = "data/entities/portraits/thanis_full.png",
    battle_portraits = "data/entities/portraits/battle/thanis_damage.png",
    stamina_icon = "data/battles/stamina_icons/characters/thanis.png",
    map_sprite_name = "Thanis",
    special_skill_category_name = vt_system.Translate("Acheron"),
    special_skill_category_icon = "data/gui/battle/acheron.png",

    battle_animations = {
        idle = "data/entities/battle/characters/thanis_idle.lua",
        run = "data/entities/battle/characters/thanis_run.lua",
        run_after_victory = "data/entities/battle/characters/thanis_run.lua",
        run_left = "data/entities/battle/characters/thanis_run_left.lua",
        attack = "data/entities/battle/characters/thanis_attack.lua",
        dodge = "data/entities/battle/characters/thanis_idle.lua",
        victory = "data/entities/battle/characters/thanis_victory.lua",
        dying = "data/entities/battle/characters/thanis_dead.lua",
        dead = "data/entities/battle/characters/thanis_dead.lua",
        revive = "data/entities/battle/characters/thanis_idle.lua",
        item = "data/entities/battle/characters/thanis_idle.lua",
        magic_prepare = "data/entities/battle/characters/thanis_idle.lua",
        magic_cast = "data/entities/battle/characters/thanis_idle.lua"
    },

    initial_stats = {
        experience_level = 18,
        experience_points = 6728,
        max_hit_points = 237,
        max_skill_points = 25,
        phys_atk = 45,
        mag_atk = 21,
        phys_def = 47,
        mag_def = 23,
        stamina = 46,
        evade = 7.0,
        weapon = 10003,
        head_armor = 20013,
        torso_armor = 30013,
        arm_armor = 40013,
        leg_armor = 50012
    },

    attack_points = {
        [vt_global.GameGlobal.GLOBAL_POSITION_HEAD] = {
            name = vt_system.Translate("Head"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = -0.20,
            mag_def_modifier = 0.25,
            evade_modifier = 0.50,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_TORSO] = {
            name = vt_system.Translate("Torso"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.40,
            mag_def_modifier = 0.10,
            evade_modifier = -0.20
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_ARMS] = {
            name = vt_system.Translate("Arms"),
            x_position = 31,
            y_position = 54,
            phys_def_modifier = 0.10,
            mag_def_modifier = 0.00,
            evade_modifier = 0.10,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK] = 10.0 }
        },
        [vt_global.GameGlobal.GLOBAL_POSITION_LEGS] = {
            name = vt_system.Translate("Legs"),
            x_position = 37,
            y_position = 34,
            phys_def_modifier = 0.20,
            mag_def_modifier = 0.20,
            evade_modifier = 0.05,
            status_effects = { [vt_global.GameGlobal.GLOBAL_STATUS_STAMINA] = 10.0 }
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

        phys_atk = {
            2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 5, 5, 5, 5, 5
        },

        mag_atk = {
            1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
            2, 2, 2, 2, 3, 3, 3, 3, 3, 3
        },

        phys_def = {
            2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 4, 4, 4, 4, 4, 4
        },

        mag_def = {
            1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3
        },

        stamina = {
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0
        },

        evade = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.35, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.35, 0.0, 0.0, 0.0
        }
    },

    -- The default skill available when no weapon.
    bare_hands_skills = { 30001 },

    skills = { [1] = 1, [6] = 300, [12] = 10009, [24] = 10010, [48] = 301, [96] = 10011 }
} -- characters[THANIS]

------------------------------------------------------------------------------[[
-- \brief Sets the growth data for a character when they have gained a level
-- \param character A pointer to the GlobalCharacter object to act on
--
-- Before this function is called, the character should already have their
-- _experience_level member incremented to the new level. What this function does
-- is determine the amount that each stat will grow by on the current level gained.
-- This function should be called every time a
-- character gains a level, and also when a new character is constructed from an
-- initial state.
------------------------------------------------------------------------------]]
function DetermineLevelGrowth(character)
    local new_level = character:GetExperienceLevel() - 1;    -- The value of the character's new XP level
    local character_table = characters[character:GetID()];   -- Reference to the character's definition table
    local growth_table = nil;                                -- Reference to the table containing the character's growth stats

    if (character_table == nil) then
        print("LUA ERROR: characters.lua::DetermineLevelGrowth() failed because the character's ID was invalid");
        return;
    end

    growth_table = character_table["growth"];
    if (growth_table == nil) then
        print("LUA ERROR: characters.lua::DetermineLevelGrowth() failed because no growth table for the character was found");
        return;
    end

    -- All growth members should be zero when this function is called. Warn if this is not the case
    if (character._hit_points_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when hit_points_growth was non-zero.");
    end
    if (character._skill_points_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when skill_points_growth was non-zero.");
    end
    if (character._phys_atk_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when phys_atk_growth was non-zero.");
    end
    if (character._mag_atk_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when mag_atk_growth was non-zero.");
    end
    if (character._phys_def_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when phys_def_growth was non-zero.");
    end
    if (character._mag_def_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when mag_def_growth was non-zero.");
    end
    if (character._stamina_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when stamina_growth was non-zero.");
    end
    if (character._evade_growth ~= 0) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() called when evade_growth was non-zero.");
    end

    -- Testing at least one stat to see whether the table has still got values
    if (growth_table["hit_points"][new_level] == nil) then
        print("LUA WARN: character.lua:DetermineLevelGrowth() next level data unexisting. Can't properly level up the character.");
        character._hit_points_growth = 1;
        character:AddExperienceForNextLevel(500);
        -- Remove the level up in that case
        character:SetExperienceLevel(character:GetExperienceLevel() - 1);
        return;
    end

    -- Copy over the character's stat growth data
    character._hit_points_growth = growth_table["hit_points"][new_level];
    character._skill_points_growth = growth_table["skill_points"][new_level];
    character._phys_atk_growth = growth_table["phys_atk"][new_level];
    character._mag_atk_growth = growth_table["mag_atk"][new_level];
    character._phys_def_growth = growth_table["phys_def"][new_level];
    character._mag_def_growth = growth_table["mag_def"][new_level];
    character._stamina_growth = growth_table["stamina"][new_level];
    character._evade_growth = growth_table["evade"][new_level];

    character:AddExperienceForNextLevel(growth_table["experience_for_next_level"][new_level]);
end -- function DetermineLevelGrowth(character)


------------------------------------------------------------------------------[[
-- \brief Adds any learned skills for a character based on their current experience level
-- \param character A pointer to the GlobalCharacter object to act on
--
-- Before this function is called, the character should already have their
-- _experience_level member incremented to the new level. What this function does
-- is determine the amount that each stat will grow by on the current level gained
-- and if any new skills will be learned by reaching this level.
------------------------------------------------------------------------------]]
function DetermineNewSkillsLearned(character)
    local new_level = character:GetExperienceLevel();        -- The value of the character's new XP level
    local character_table = characters[character:GetID()];   -- Reference to the character's definition table
    local new_skills = nil;                                  -- Reference to the number or table of the new skills learned

    if (character_table == nil) then
        print("LUA ERROR: characters.lua::DeterminedNewSkillsLearned() failed because the character's ID was invalid");
        return;
    end

    new_skills = character_table["skills"][new_level];

    -- Case 1: no new skills are learned
    if (type(new_skills) == "nil") then
        return;
    -- Case 2: one new skill is learned
    elseif (type(new_skills) == "number") then
        character:AddNewSkillLearned(new_skills);
    -- Case 3: multiple new skills are learned
    elseif (type(new_skills) == "table") then
        for i, skill in ipairs(new_skills) do
            character:AddNewSkillLearned(skill);
        end
    else
        print("LUA ERROR: characters.lua::DetermineNewSkillsLearned() failed because of an unexpected skill table key type");
    end
end -- function DetermineSkillsLearned(character)
