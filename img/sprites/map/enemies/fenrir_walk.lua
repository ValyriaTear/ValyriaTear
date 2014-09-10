-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/enemies/fenrir_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 6,
	columns = 11,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 22, duration = 100 },
            [1] = { id = 23, duration = 100 },
            [2] = { id = 24, duration = 100 },
            [3] = { id = 25, duration = 100 },
            [4] = { id = 26, duration = 100 },
            [5] = { id = 27, duration = 100 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 33, duration = 100 },
            [1] = { id = 34, duration = 100 },
            [2] = { id = 35, duration = 100 },
            [3] = { id = 36, duration = 100 },
            [4] = { id = 37, duration = 100 },
            [5] = { id = 38, duration = 100 }
        },
        [ANIM_WEST] = {
            [0] = { id = 0, duration = 100 },
            [1] = { id = 1, duration = 100 },
            [2] = { id = 2, duration = 100 },
            [3] = { id = 3, duration = 100 },
            [4] = { id = 4, duration = 100 },
            [5] = { id = 5, duration = 100 }
        },
        [ANIM_EAST] = {
            [0] = { id = 11, duration = 100 },
            [1] = { id = 12, duration = 100 },
            [2] = { id = 13, duration = 100 },
            [3] = { id = 14, duration = 100 },
            [4] = { id = 15, duration = 100 },
            [5] = { id = 16, duration = 100 }
        }
    }
}
