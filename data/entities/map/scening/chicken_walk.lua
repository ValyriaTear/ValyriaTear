-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/scening/chicken_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 8,
	columns = 4,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 24, duration = 150 },
            [1] = { id = 25, duration = 150 },
            [2] = { id = 26, duration = 150 },
            [3] = { id = 27, duration = 150 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 16, duration = 150 },
            [1] = { id = 17, duration = 150 },
            [2] = { id = 18, duration = 150 },
            [3] = { id = 19, duration = 150 }
        },
        [ANIM_WEST] = {
            [0] = { id = 20, duration = 150 },
            [1] = { id = 21, duration = 150 },
            [2] = { id = 22, duration = 150 },
            [3] = { id = 23, duration = 150 }
        },
        [ANIM_EAST] = {
            [0] = { id = 28, duration = 150 },
            [1] = { id = 29, duration = 150 },
            [2] = { id = 30, duration = 150 },
            [3] = { id = 31, duration = 150 }
        }
    }
}
