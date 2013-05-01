-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/characters/kalya_run.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 4,
	columns = 6,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 1, duration = 130 },
            [1] = { id = 2, duration = 130 },
            [2] = { id = 3, duration = 130 },
            [3] = { id = 1, duration = 130 },
            [4] = { id = 4, duration = 130 },
            [5] = { id = 5, duration = 130 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 7, duration = 130 },
            [1] = { id = 8, duration = 130 },
            [2] = { id = 9, duration = 130 },
            [3] = { id = 7, duration = 130 },
            [4] = { id = 10, duration = 130 },
            [5] = { id = 11, duration = 130 }
        },
        [ANIM_WEST] = {
            [0] = { id = 13, duration = 130 },
            [1] = { id = 14, duration = 130 },
            [2] = { id = 15, duration = 130 },
            [3] = { id = 13, duration = 130 },
            [4] = { id = 16, duration = 130 },
            [5] = { id = 17, duration = 130 }
        },
        [ANIM_EAST] = {
            [0] = { id = 19, duration = 130 },
            [1] = { id = 20, duration = 130 },
            [2] = { id = 21, duration = 130 },
            [3] = { id = 19, duration = 130 },
            [4] = { id = 22, duration = 130 },
            [5] = { id = 23, duration = 130 }
        }
    }
}
