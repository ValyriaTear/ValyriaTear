-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;


sprite_animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/enemies/ratto_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 10,
    columns = 7,
    -- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 7, duration = 150 },
            [1] = { id = 9, duration = 150 },
            [2] = { id = 11, duration = 150 },
            [3] = { id = 12, duration = 150 },
            [4] = { id = 13, duration = 150 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 35, duration = 150 },
            [1] = { id = 37, duration = 150 },
            [2] = { id = 39, duration = 150 },
            [3] = { id = 40, duration = 150 },
            [4] = { id = 41, duration = 150 }
        },
        [ANIM_WEST] = {
            [0] = { id = 49, duration = 150 },
            [1] = { id = 51, duration = 150 },
            [2] = { id = 53, duration = 150 },
            [3] = { id = 54, duration = 150 },
            [4] = { id = 55, duration = 150 }
        },
        [ANIM_EAST] = {
            [0] = { id = 21, duration = 150 },
            [1] = { id = 23, duration = 150 },
            [2] = { id = 25, duration = 150 },
            [3] = { id = 26, duration = 150 },
            [4] = { id = 27, duration = 150 },
        }
    }
}
