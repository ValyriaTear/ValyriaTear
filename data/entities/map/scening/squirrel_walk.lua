-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/scening/squirrel_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 8,
	columns = 8,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 48, duration = 110 },
            [1] = { id = 49, duration = 110 },
            [2] = { id = 50, duration = 110 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 56, duration = 110 },
            [1] = { id = 57, duration = 110 },
            [2] = { id = 58, duration = 110 }
        },
        [ANIM_WEST] = {
            [0] = { id = 40, duration = 110 },
            [1] = { id = 41, duration = 110 },
            [2] = { id = 42, duration = 110 }
        },
        [ANIM_EAST] = {
            [0] = { id = 32, duration = 110 },
            [1] = { id = 33, duration = 110 },
            [2] = { id = 34, duration = 110 }
        }
    }
}
