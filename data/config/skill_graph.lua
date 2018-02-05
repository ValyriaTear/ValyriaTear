-- Describes all the skills and other improvements in the skill graph the characters can get.
-- Each entries are nodes that are linked to a list of other nodes, permiting to set paths between them.
-- Cross a path costs experience points, items, or both.
-- And also the character's start location.
BRONANN  = 1;
KALYA    = 2;
SYLVE    = 4;
THANIS   = 8;

skill_graph_start = {
	-- [Character id] = node id
	[BRONANN] = 0,
	[KALYA] = 0,
	[SYLVE] = 0,
	[THANIS] = 0
}

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
		x_location = 290,
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
			26,
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
			28,
		},
	},
	[28] = {
		x_location = 883,
		y_location = 504,
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
			29,
		},
	},
	[29] = {
		x_location = 821,
		y_location = 560,
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
			30,
		},
	},
	[30] = {
		x_location = 852,
		y_location = 646,
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
			31,
		},
	},
	[31] = {
		x_location = 960,
		y_location = 659,
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
			32,
		},
	},
	[32] = {
		x_location = 1030,
		y_location = 609,
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
			33,
		},
	},
	[33] = {
		x_location = 1074,
		y_location = 534,
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
			34,62,
		},
	},
	[34] = {
		x_location = 1040,
		y_location = 447,
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
			35,47,
		},
	},
	[35] = {
		x_location = 959,
		y_location = 419,
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
			36,
		},
	},
	[36] = {
		x_location = 865,
		y_location = 424,
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
			37,
		},
	},
	[37] = {
		x_location = 803,
		y_location = 446,
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
			38,
		},
	},
	[38] = {
		x_location = 757,
		y_location = 523,
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
			39,52,
		},
	},
	[39] = {
		x_location = 748,
		y_location = 625,
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
			40,
		},
	},
	[40] = {
		x_location = 818,
		y_location = 726,
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
			41,57,
		},
	},
	[41] = {
		x_location = 906,
		y_location = 759,
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
			42,73,
		},
	},
	[42] = {
		x_location = 989,
		y_location = 745,
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
			43,74,
		},
	},
	[43] = {
		x_location = 1076,
		y_location = 728,
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
			44,
		},
	},
	[44] = {
		x_location = 1148,
		y_location = 662,
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
			45,
		},
	},
	[45] = {
		x_location = 1191,
		y_location = 586,
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
			46,
		},
	},
	[46] = {
		x_location = 1223,
		y_location = 507,
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
	[47] = {
		x_location = 1083,
		y_location = 384,
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
			48,
		},
	},
	[48] = {
		x_location = 1002,
		y_location = 341,
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
			49,
		},
	},
	[49] = {
		x_location = 912,
		y_location = 330,
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
			50,
		},
	},
	[50] = {
		x_location = 820,
		y_location = 350,
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
			51,
		},
	},
	[51] = {
		x_location = 740,
		y_location = 389,
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
			52,
		},
	},
	[52] = {
		x_location = 682,
		y_location = 458,
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
			53,60,
		},
	},
	[53] = {
		x_location = 658,
		y_location = 543,
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
			54,
		},
	},
	[54] = {
		x_location = 641,
		y_location = 614,
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
			55,
		},
	},
	[55] = {
		x_location = 644,
		y_location = 697,
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
			56,
		},
	},
	[56] = {
		x_location = 694,
		y_location = 773,
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
			57,
		},
	},
	[57] = {
		x_location = 783,
		y_location = 836,
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
			58,70,
		},
	},
	[58] = {
		x_location = 882,
		y_location = 890,
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
			59,
		},
	},
	[59] = {
		x_location = 989,
		y_location = 936,
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
	[60] = {
		x_location = 619,
		y_location = 406,
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
			61,
		},
	},
	[61] = {
		x_location = 520,
		y_location = 378,
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
	[62] = {
		x_location = 1153,
		y_location = 475,
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
			63,
		},
	},
	[63] = {
		x_location = 1238,
		y_location = 402,
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
			71,
		},
	},
	[71] = {
		x_location = 738,
		y_location = 1008,
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
			72,
		},
	},
	[72] = {
		x_location = 763,
		y_location = 1089,
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
	[73] = {
		x_location = 908,
		y_location = 849,
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
			74,
		},
	},
	[74] = {
		x_location = 1009,
		y_location = 855,
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
			77,75,
		},
	},
	[75] = {
		x_location = 1099,
		y_location = 841,
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
			76,
		},
	},
	[76] = {
		x_location = 1168,
		y_location = 793,
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
	[77] = {
		x_location = 1120,
		y_location = 929,
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
}
