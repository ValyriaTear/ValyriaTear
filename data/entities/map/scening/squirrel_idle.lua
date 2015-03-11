-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/scening/squirrel_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 8,
	columns = 8,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 16, duration = 375 },
            [1] = { id = 17, duration = 375 },
            [2] = { id = 18, duration = 375 },
            [3] = { id = 19, duration = 375 },
            [4] = { id = 20, duration = 75 },
            [5] = { id = 21, duration = 75 },
            [6] = { id = 22, duration = 75 },
            [7] = { id = 23, duration = 75 },
            [8] = { id = 24, duration = 75 },
            [9] = { id = 25, duration = 75 },
            [10] = { id = 16, duration = 120 },
            [11] = { id = 17, duration = 120 },
            [12] = { id = 18, duration = 120 },
            [13] = { id = 19, duration = 120 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 24, duration = 375 },
            [1] = { id = 25, duration = 375 },
            [2] = { id = 26, duration = 375 },
            [3] = { id = 27, duration = 375 },
            [4] = { id = 28, duration = 75 },
            [5] = { id = 29, duration = 75 },
            [6] = { id = 30, duration = 75 },
            [7] = { id = 31, duration = 75 },
            [8] = { id = 24, duration = 75 },
            [9] = { id = 25, duration = 75 },
            [10] = { id = 26, duration = 75 },
            [11] = { id = 27, duration = 75 }
        },
        [ANIM_WEST] = {
            [0] = { id = 8, duration = 375 },
            [1] = { id = 9, duration = 375 },
            [2] = { id = 10, duration = 375 },
            [3] = { id = 11, duration = 375 },
            [4] = { id = 12, duration = 75 },
            [5] = { id = 13, duration = 75 },
            [6] = { id = 14, duration = 75 },
            [7] = { id = 15, duration = 75 },
            [8] = { id = 8, duration = 75 },
            [9] = { id = 9, duration = 75 },
            [10] = { id = 10, duration = 75 },
            [11] = { id = 11, duration = 75 }
        },
        [ANIM_EAST] = {
            [0] = { id = 0, duration = 375 },
            [1] = { id = 1, duration = 375 },
            [2] = { id = 2, duration = 375 },
            [3] = { id = 3, duration = 375 },
            [4] = { id = 4, duration = 75 },
            [5] = { id = 5, duration = 75 },
            [6] = { id = 6, duration = 75 },
            [7] = { id = 7, duration = 75 },
            [8] = { id = 0, duration = 75 },
            [9] = { id = 1, duration = 75 },
            [10] = { id = 2, duration = 75 },
            [11] = { id = 3, duration = 75 }
        }
    }
}
