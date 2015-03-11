-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;


sprite_animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/characters/kalya_walk.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 4,
	columns = 6,
	-- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 0, duration = 150 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 6, duration = 150 }
        },
        [ANIM_WEST] = {
            [0] = { id = 12, duration = 150 }
        },
        [ANIM_EAST] = {
            [0] = { id = 18, duration = 150 }
        }
    }
}
