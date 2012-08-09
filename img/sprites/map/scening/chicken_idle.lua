-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = hoa_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = hoa_map.MapMode.ANIM_NORTH;
local ANIM_WEST = hoa_map.MapMode.ANIM_WEST;
local ANIM_EAST = hoa_map.MapMode.ANIM_EAST;

sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/scening/chicken_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 8,
	columns = 4,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 8, duration = 3000 },
            [1] = { id = 9, duration = 150 },
            [2] = { id = 10, duration = 75 },
            [3] = { id = 9, duration = 150 },
            [4] = { id = 10, duration = 75 },
            [5] = { id = 9, duration = 1000 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 0, duration = 3000 },
            [1] = { id = 1, duration = 150 },
            [2] = { id = 2, duration = 75 },
            [3] = { id = 1, duration = 150 },
            [4] = { id = 2, duration = 75 },
            [5] = { id = 1, duration = 1000 }
        },
        [ANIM_WEST] = {
            [0] = { id = 4, duration = 3000 },
            [1] = { id = 5, duration = 150 },
            [2] = { id = 6, duration = 75 },
            [3] = { id = 5, duration = 150 },
            [4] = { id = 6, duration = 75 },
            [5] = { id = 5, duration = 1000 }
        },
        [ANIM_EAST] = {
            [0] = { id = 12, duration = 3000 },
            [1] = { id = 13, duration = 150 },
            [2] = { id = 14, duration = 75 },
            [3] = { id = 13, duration = 150 },
            [4] = { id = 14, duration = 75 },
            [5] = { id = 13, duration = 1000 }
        }
    }
}
