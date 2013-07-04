-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/enemies/spiky_mushroom.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 4,
	columns = 10,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 1, duration = 75 },
            [1] = { id = 2, duration = 75 },
            [2] = { id = 3, duration = 75 },
            [3] = { id = 4, duration = 75 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 21, duration = 75 },
            [1] = { id = 22, duration = 75 },
            [2] = { id = 23, duration = 75 },
            [3] = { id = 24, duration = 75 }
        },
        [ANIM_WEST] = {
            [0] = { id = 11, duration = 75 },
            [1] = { id = 12, duration = 75 },
            [2] = { id = 13, duration = 75 },
            [3] = { id = 14, duration = 75 }
        },
        [ANIM_EAST] = {
            [0] = { id = 31, duration = 75 },
            [1] = { id = 32, duration = 75 },
            [2] = { id = 33, duration = 75 },
            [3] = { id = 34, duration = 75 }
        }
    }
}
