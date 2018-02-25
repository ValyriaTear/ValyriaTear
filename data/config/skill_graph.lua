-- Describes all the skills and other improvements in the skill graph the characters can get.
-- Each entries are nodes that are linked to a list of other nodes, permiting to set paths between them.
-- Cross a path costs experience points, items, or both.
-- And also the character's start location.

skill_graph = {
	[0] = {
		x_location = 529,
		y_location = 51,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			1,5,
		},
	},
	[1] = {
		x_location = 575,
		y_location = 101,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			2,
		},
	},
	[2] = {
		x_location = 611,
		y_location = 161,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			3,
		},
	},
	[3] = {
		x_location = 660,
		y_location = 232,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[4] = {
		x_location = 721,
		y_location = 278,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			3,
		},
	},
	[5] = {
		x_location = 470,
		y_location = 103,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			6,
		},
	},
	[6] = {
		x_location = 404,
		y_location = 183,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			7,
		},
	},
	[7] = {
		x_location = 354,
		y_location = 253,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			8,
		},
	},
	[8] = {
		x_location = 287,
		y_location = 347,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			9,
		},
	},
	[9] = {
		x_location = 234,
		y_location = 426,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			10,18,19,
		},
	},
	[10] = {
		x_location = 209,
		y_location = 529,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			11,
		},
	},
	[11] = {
		x_location = 177,
		y_location = 615,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			12,
		},
	},
	[12] = {
		x_location = 150,
		y_location = 715,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			13,
		},
	},
	[13] = {
		x_location = 128,
		y_location = 812,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			14,
		},
	},
	[14] = {
		x_location = 121,
		y_location = 893,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			15,
		},
	},
	[15] = {
		x_location = 116,
		y_location = 985,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			16,
		},
	},
	[16] = {
		x_location = 113,
		y_location = 1100,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			17,
		},
	},
	[17] = {
		x_location = 177,
		y_location = 1185,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[18] = {
		x_location = 146,
		y_location = 455,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[19] = {
		x_location = 342,
		y_location = 394,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			20,
		},
	},
	[20] = {
		x_location = 331,
		y_location = 495,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			21,
		},
	},
	[21] = {
		x_location = 320,
		y_location = 589,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			22,
		},
	},
	[22] = {
		x_location = 310,
		y_location = 705,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			23,
		},
	},
	[23] = {
		x_location = 306,
		y_location = 798,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			24,
		},
	},
	[24] = {
		x_location = 301,
		y_location = 889,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			25,
		},
	},
	[25] = {
		x_location = 291,
		y_location = 981,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			26,177,
		},
	},
	[26] = {
		x_location = 289,
		y_location = 1062,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[27] = {
		x_location = 957,
		y_location = 515,
		icon_file = "data/gui/menus/socket.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			28,
		},
	},
	[28] = {
		x_location = 883,
		y_location = 504,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 16,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			29,
		},
	},
	[29] = {
		x_location = 821,
		y_location = 560,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			30,
		},
	},
	[30] = {
		x_location = 852,
		y_location = 646,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			31,
		},
	},
	[31] = {
		x_location = 960,
		y_location = 659,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 16,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			32,
		},
	},
	[32] = {
		x_location = 1030,
		y_location = 609,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			33,
		},
	},
	[33] = {
		x_location = 1074,
		y_location = 534,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 16,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			34,62,
		},
	},
	[34] = {
		x_location = 1040,
		y_location = 447,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 14,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			35,47,
		},
	},
	[35] = {
		x_location = 959,
		y_location = 419,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			36,
		},
	},
	[36] = {
		x_location = 865,
		y_location = 424,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			37,
		},
	},
	[37] = {
		x_location = 803,
		y_location = 446,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			38,
		},
	},
	[38] = {
		x_location = 757,
		y_location = 523,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			39,52,
		},
	},
	[39] = {
		x_location = 748,
		y_location = 625,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			40,
		},
	},
	[40] = {
		x_location = 818,
		y_location = 726,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 21,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			41,57,
		},
	},
	[41] = {
		x_location = 906,
		y_location = 759,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			42,73,
		},
	},
	[42] = {
		x_location = 989,
		y_location = 745,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			43,74,
		},
	},
	[43] = {
		x_location = 1076,
		y_location = 728,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 25,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			44,
		},
	},
	[44] = {
		x_location = 1151,
		y_location = 663,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			45,171,
		},
	},
	[45] = {
		x_location = 1191,
		y_location = 586,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			46,
		},
	},
	[46] = {
		x_location = 1223,
		y_location = 507,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 25,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 13,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[47] = {
		x_location = 1083,
		y_location = 384,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			48,
		},
	},
	[48] = {
		x_location = 1002,
		y_location = 341,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			49,
		},
	},
	[49] = {
		x_location = 912,
		y_location = 330,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			50,
		},
	},
	[50] = {
		x_location = 820,
		y_location = 350,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			51,
		},
	},
	[51] = {
		x_location = 740,
		y_location = 389,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			52,
		},
	},
	[52] = {
		x_location = 682,
		y_location = 458,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			53,60,
		},
	},
	[53] = {
		x_location = 658,
		y_location = 543,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			54,
		},
	},
	[54] = {
		x_location = 641,
		y_location = 614,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			55,
		},
	},
	[55] = {
		x_location = 647,
		y_location = 699,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			56,185,
		},
	},
	[56] = {
		x_location = 694,
		y_location = 773,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 25,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			57,
		},
	},
	[57] = {
		x_location = 783,
		y_location = 836,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 13,
		},
		-- links with other nodes
		links = {
			58,70,
		},
	},
	[58] = {
		x_location = 882,
		y_location = 890,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			59,
		},
	},
	[59] = {
		x_location = 989,
		y_location = 936,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[60] = {
		x_location = 619,
		y_location = 406,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			61,
		},
	},
	[61] = {
		x_location = 520,
		y_location = 378,
		icon_file = "data/entities/status_effects/evade_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 150,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[5] = 10,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[62] = {
		x_location = 1153,
		y_location = 475,
		icon_file = "data/gui/menus/star.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = 2,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			63,
		},
	},
	[63] = {
		x_location = 1238,
		y_location = 402,
		icon_file = "data/gui/menus/rotating_crystal_grey.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
			[3001] = 1,
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			64,
		},
	},
	[64] = {
		x_location = 1295,
		y_location = 460,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			65,
		},
	},
	[65] = {
		x_location = 1339,
		y_location = 545,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			66,
		},
	},
	[66] = {
		x_location = 1367,
		y_location = 624,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			67,
		},
	},
	[67] = {
		x_location = 1394,
		y_location = 706,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			68,78,
		},
	},
	[68] = {
		x_location = 1405,
		y_location = 776,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			69,
		},
	},
	[69] = {
		x_location = 1418,
		y_location = 857,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[70] = {
		x_location = 755,
		y_location = 916,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			71,
		},
	},
	[71] = {
		x_location = 738,
		y_location = 1008,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 3,
		},
		-- links with other nodes
		links = {
			72,
		},
	},
	[72] = {
		x_location = 763,
		y_location = 1089,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[73] = {
		x_location = 908,
		y_location = 849,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 1,
		},
		-- links with other nodes
		links = {
			74,
		},
	},
	[74] = {
		x_location = 1009,
		y_location = 855,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 25,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			77,75,
		},
	},
	[75] = {
		x_location = 1099,
		y_location = 841,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			76,
		},
	},
	[76] = {
		x_location = 1168,
		y_location = 793,
		icon_file = "data/gui/menus/star.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 115,
		-- Skill id learned when reaching this node
		skill_id_learned = 3,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[77] = {
		x_location = 1050,
		y_location = 930,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 25,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[78] = {
		x_location = 1462,
		y_location = 690,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			79,
		},
	},
	[79] = {
		x_location = 1517,
		y_location = 629,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			80,
		},
	},
	[80] = {
		x_location = 1561,
		y_location = 574,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			81,
		},
	},
	[81] = {
		x_location = 1619,
		y_location = 525,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			82,
		},
	},
	[82] = {
		x_location = 1661,
		y_location = 474,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			93,83,86,
		},
	},
	[83] = {
		x_location = 1656,
		y_location = 544,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			84,
		},
	},
	[84] = {
		x_location = 1653,
		y_location = 639,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			85,
		},
	},
	[85] = {
		x_location = 1624,
		y_location = 704,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[86] = {
		x_location = 1653,
		y_location = 391,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			87,
		},
	},
	[87] = {
		x_location = 1628,
		y_location = 321,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			88,
		},
	},
	[88] = {
		x_location = 1578,
		y_location = 261,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			89,
		},
	},
	[89] = {
		x_location = 1510,
		y_location = 254,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			90,
		},
	},
	[90] = {
		x_location = 1464,
		y_location = 315,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			91,
		},
	},
	[91] = {
		x_location = 1511,
		y_location = 408,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			92,
		},
	},
	[92] = {
		x_location = 1582,
		y_location = 391,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			87,
		},
	},
	[93] = {
		x_location = 1705,
		y_location = 418,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[94] = {
		x_location = 1376,
		y_location = 313,
		icon_file = "data/gui/menus/star.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 49,
		-- Skill id learned when reaching this node
		skill_id_learned = 10001,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[95] = {
		x_location = 1463,
		y_location = 250,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 2,
		},
		-- links with other nodes
		links = {
			94,
		},
	},
	[96] = {
		x_location = 1541,
		y_location = 200,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 13,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			95,
		},
	},
	[97] = {
		x_location = 1599,
		y_location = 156,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			96,
		},
	},
	[98] = {
		x_location = 1672,
		y_location = 111,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 14,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			97,
		},
	},
	[99] = {
		x_location = 1737,
		y_location = 58,
		icon_file = "data/gui/menus/socket.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			98,100,
		},
	},
	[100] = {
		x_location = 1811,
		y_location = 141,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			101,
		},
	},
	[101] = {
		x_location = 1850,
		y_location = 229,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 15,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			102,
		},
	},
	[102] = {
		x_location = 1888,
		y_location = 330,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			103,113,114,
		},
	},
	[103] = {
		x_location = 1922,
		y_location = 431,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			104,
		},
	},
	[104] = {
		x_location = 1943,
		y_location = 517,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 2,
		},
		-- links with other nodes
		links = {
			105,
		},
	},
	[105] = {
		x_location = 1951,
		y_location = 634,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 17,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			106,
		},
	},
	[106] = {
		x_location = 1946,
		y_location = 744,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			107,
		},
	},
	[107] = {
		x_location = 1935,
		y_location = 841,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			108,
		},
	},
	[108] = {
		x_location = 1924,
		y_location = 937,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			109,
		},
	},
	[109] = {
		x_location = 1911,
		y_location = 1027,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 2,
		},
		-- links with other nodes
		links = {
			110,
		},
	},
	[110] = {
		x_location = 1893,
		y_location = 1101,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			111,
		},
	},
	[111] = {
		x_location = 1865,
		y_location = 1168,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			112,
		},
	},
	[112] = {
		x_location = 1804,
		y_location = 1227,
		icon_file = "data/gui/menus/star.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 102,
		-- Skill id learned when reaching this node
		skill_id_learned = 10002,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[113] = {
		x_location = 1952,
		y_location = 276,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 14,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[114] = {
		x_location = 1813,
		y_location = 373,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			115,
		},
	},
	[115] = {
		x_location = 1854,
		y_location = 466,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 19,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			116,
		},
	},
	[116] = {
		x_location = 1866,
		y_location = 550,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			117,
		},
	},
	[117] = {
		x_location = 1863,
		y_location = 643,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			118,
		},
	},
	[118] = {
		x_location = 1853,
		y_location = 750,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			119,
		},
	},
	[119] = {
		x_location = 1831,
		y_location = 834,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			120,
		},
	},
	[120] = {
		x_location = 1807,
		y_location = 933,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 18,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			121,154,155,
		},
	},
	[121] = {
		x_location = 1787,
		y_location = 1036,
		icon_file = "data/entities/status_effects/evade_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[5] = 4,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[122] = {
		x_location = 824,
		y_location = 1024,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			123,
		},
	},
	[123] = {
		x_location = 870,
		y_location = 1124,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			124,
		},
	},
	[124] = {
		x_location = 894,
		y_location = 1236,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			125,
		},
	},
	[125] = {
		x_location = 894,
		y_location = 1346,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			126,
		},
	},
	[126] = {
		x_location = 887,
		y_location = 1432,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			127,
		},
	},
	[127] = {
		x_location = 884,
		y_location = 1528,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			150,128,
		},
	},
	[128] = {
		x_location = 861,
		y_location = 1599,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			129,
		},
	},
	[129] = {
		x_location = 845,
		y_location = 1680,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			130,
		},
	},
	[130] = {
		x_location = 803,
		y_location = 1771,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[131] = {
		x_location = 1067,
		y_location = 1125,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			132,
		},
	},
	[132] = {
		x_location = 1073,
		y_location = 1237,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			133,
		},
	},
	[133] = {
		x_location = 1067,
		y_location = 1348,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			134,
		},
	},
	[134] = {
		x_location = 1076,
		y_location = 1430,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			135,
		},
	},
	[135] = {
		x_location = 1071,
		y_location = 1523,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			151,136,
		},
	},
	[136] = {
		x_location = 1074,
		y_location = 1609,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[137] = {
		x_location = 1234,
		y_location = 986,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			138,
		},
	},
	[138] = {
		x_location = 1217,
		y_location = 1103,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			139,
		},
	},
	[139] = {
		x_location = 1212,
		y_location = 1224,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			140,
		},
	},
	[140] = {
		x_location = 1214,
		y_location = 1333,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			141,
		},
	},
	[141] = {
		x_location = 1222,
		y_location = 1426,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			142,
		},
	},
	[142] = {
		x_location = 1229,
		y_location = 1529,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			152,143,
		},
	},
	[143] = {
		x_location = 1245,
		y_location = 1618,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			144,
		},
	},
	[144] = {
		x_location = 1272,
		y_location = 1695,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			145,
		},
	},
	[145] = {
		x_location = 1307,
		y_location = 1774,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[146] = {
		x_location = 795,
		y_location = 1528,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			127,
		},
	},
	[147] = {
		x_location = 713,
		y_location = 1555,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			146,
		},
	},
	[148] = {
		x_location = 653,
		y_location = 1605,
		icon_file = "data/gui/menus/rotating_crystal_grey.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
			[3001] = 1,
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			147,183,
		},
	},
	[149] = {
		x_location = 598,
		y_location = 1645,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			148,
		},
	},
	[150] = {
		x_location = 997,
		y_location = 1525,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			135,
		},
	},
	[151] = {
		x_location = 1157,
		y_location = 1523,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			142,
		},
	},
	[152] = {
		x_location = 1341,
		y_location = 1532,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			162,153,
		},
	},
	[153] = {
		x_location = 1425,
		y_location = 1577,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[154] = {
		x_location = 1874,
		y_location = 900,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 21,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[155] = {
		x_location = 1739,
		y_location = 960,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			156,
		},
	},
	[156] = {
		x_location = 1670,
		y_location = 1034,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 20,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 2,
		},
		-- links with other nodes
		links = {
			157,
		},
	},
	[157] = {
		x_location = 1630,
		y_location = 1100,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 2,
		},
		-- links with other nodes
		links = {
			158,
		},
	},
	[158] = {
		x_location = 1570,
		y_location = 1200,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 24,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			159,
		},
	},
	[159] = {
		x_location = 1458,
		y_location = 1280,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 5,
		},
		-- links with other nodes
		links = {
			160,163,164,
		},
	},
	[160] = {
		x_location = 1390,
		y_location = 1320,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 22,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			161,
		},
	},
	[161] = {
		x_location = 1374,
		y_location = 1458,
		icon_file = "data/entities/status_effects/def_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[2] = 2,
		},
		-- links with other nodes
		links = {
			152,
		},
	},
	[162] = {
		x_location = 1333,
		y_location = 1634,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 22,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 1,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[163] = {
		x_location = 1504,
		y_location = 1367,
		icon_file = "data/entities/status_effects/stamina_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 23,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[4] = 1,
		},
		-- links with other nodes
		links = {
			
		},
	},
	[164] = {
		x_location = 1442,
		y_location = 1165,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			165,
		},
	},
	[165] = {
		x_location = 1460,
		y_location = 1047,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			166,
		},
	},
	[166] = {
		x_location = 1485,
		y_location = 941,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			167,
		},
	},
	[167] = {
		x_location = 1542,
		y_location = 867,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			168,
		},
	},
	[168] = {
		x_location = 1642,
		y_location = 800,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			169,
		},
	},
	[169] = {
		x_location = 1626,
		y_location = 897,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			170,
		},
	},
	[170] = {
		x_location = 1664,
		y_location = 967,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[171] = {
		x_location = 1212,
		y_location = 746,
		icon_file = "data/gui/menus/rotating_crystal_grey.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
			[3001] = 1,
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			172,
		},
	},
	[172] = {
		x_location = 1294,
		y_location = 828,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			173,
		},
	},
	[173] = {
		x_location = 1365,
		y_location = 901,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			166,
		},
	},
	[174] = {
		x_location = 422,
		y_location = 796,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			184,
		},
	},
	[175] = {
		x_location = 442,
		y_location = 881,
		icon_file = "data/entities/status_effects/matk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[1] = 2,
		},
		-- links with other nodes
		links = {
			174,
		},
	},
	[176] = {
		x_location = 413,
		y_location = 956,
		icon_file = "data/entities/status_effects/atk_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 40,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[0] = 3,
		},
		-- links with other nodes
		links = {
			175,
		},
	},
	[177] = {
		x_location = 372,
		y_location = 1023,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			178,
		},
	},
	[178] = {
		x_location = 453,
		y_location = 1101,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			179,
		},
	},
	[179] = {
		x_location = 513,
		y_location = 1210,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			180,
		},
	},
	[180] = {
		x_location = 558,
		y_location = 1314,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			181,
		},
	},
	[181] = {
		x_location = 597,
		y_location = 1410,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 40,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 13,
		},
		-- links with other nodes
		links = {
			182,190,
		},
	},
	[182] = {
		x_location = 630,
		y_location = 1493,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			148,
		},
	},
	[183] = {
		x_location = 686,
		y_location = 1687,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
	[184] = {
		x_location = 519,
		y_location = 859,
		icon_file = "data/entities/status_effects/hp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[6] = 13,
		},
		-- links with other nodes
		links = {
			186,
		},
	},
	[185] = {
		x_location = 560,
		y_location = 749,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 31,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 2,
		},
		-- links with other nodes
		links = {
			184,
		},
	},
	[186] = {
		x_location = 595,
		y_location = 966,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 2,
		},
		-- links with other nodes
		links = {
			187,
		},
	},
	[187] = {
		x_location = 642,
		y_location = 1077,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 30,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 1,
		},
		-- links with other nodes
		links = {
			188,
		},
	},
	[188] = {
		x_location = 649,
		y_location = 1195,
		icon_file = "data/entities/status_effects/sp_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 40,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[7] = 2,
		},
		-- links with other nodes
		links = {
			189,
		},
	},
	[189] = {
		x_location = 648,
		y_location = 1289,
		icon_file = "data/entities/status_effects/mdef_icon.lua",
		-- Experience points cost needed to reach this node
		experience_points_needed = 40,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
			[3] = 2,
		},
		-- links with other nodes
		links = {
			181,
		},
	},
	[190] = {
		x_location = 532,
		y_location = 1495,
		icon_file = "",
		-- Experience points cost needed to reach this node
		experience_points_needed = 0,
		-- Skill id learned when reaching this node
		skill_id_learned = -1,
		-- items needed to reach this node
		items_needed = {
		},
		-- stats upgrade when reaching this node
		stats = {
		},
		-- links with other nodes
		links = {
			
		},
	},
}
